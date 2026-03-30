#include "transpfuncs.h"

static ptr_createTr spCrTr;
static ptr_deleteTr spDelTr;


void RegTrFuncs(ptr_createTr pCrTr, ptr_deleteTr pDelTr)
{
    spCrTr = pCrTr;
    spDelTr = pDelTr;
}

TranspFuncs::TranspFuncs():ErrorManager()
{
    ptrTr = NULL;
    isAddrDbl = false;
    isMICS = false;
    isSCPI = false;
    pCreateTr = spCrTr;
    pDeleteTr = spDelTr;
    sweepTime_ = -1;
    moduleLabel = "";
}

TranspFuncs::~TranspFuncs()
{
    if (ptrTr&&pDeleteTr) {
        pDeleteTr(ptrTr);
        ptrTr = NULL;
    }
}


int TranspFuncs::SweepTime()
{
    SetAnsw(QString::number(sweepTime_).toStdString());
    return 0;
}


int TranspFuncs::Connect(string addr, int LibInit, string termCh)
{
    int bin = 1;
    QString VAddr=QString::fromStdString(addr);
    if (VAddr.contains("::8888::")&&!LibInit) {
        VAddr+="::MICS";
        isMICS = true;
    } else
        bin = 0;
    qDebug()<<"create transport";
    ptrTr = pCreateTr(VAddr,isAddrDbl);
    if (ptrTr) {
        if (isAddrDbl) {
            return 0;
        }
//        if(LibInit) isSCPI = true;
//        if(QString::fromStdString(addr).contains("::5025::")) isSCPI = true;
//        if(QString::fromStdString(addr).contains("ASRL")) isMICS = true;
        int code = ptrTr->InitClass(LibInit);
        if (CheckErr(code)) {
            return -1;
        }
        char termChar = termCh[0];
        termChar_ = termChar;
        code = ptrTr->Connect(VAddr.toLocal8Bit().data(),bin);
        if (CheckErr(code)) {
            qDebug()<<VAddr<<"NOT connected";
            return -1;
        }
        qDebug()<<VAddr<<" connected";
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;

}

//void TranspFuncs::setParemeter(double value, int number, std::vector<byte *> &vector)
//{`
//    __int64 IntValue = 0;
//    IntValue = (__int64)value;
//    for(int i = 0; i < 2; i++){
//        vector[number][i]=(unsigned char)(IntValue>>(i*8));
//    }
//}

int TranspFuncs::Send(string str_comm,bool ReadAnsw, bool byByte)
{
//    QTime myTimer;
//    myTimer.start();

    qDebug()<<moduleLabel<<"Send: " << QString::fromStdString(str_comm) << ",ReadAnsw=" << ReadAnsw;
    if(ptrTr)
    {
        QString comm = QString::fromStdString(str_comm);
        if(!comm.contains("\n"))
            comm+="\n";
        int cnt = comm.count();

        if (comm.contains("REGUL_CURVE_2")) {
            ptrTr->SetSuppressMICS(VI_TRUE);
            comm = comm.simplified();
            QStringList splitCommand = comm.split(" ");
            long long freq = 0;
            short type = 0;
            short temper = 0;
            long DACBegin = 0;
            long pointsNumber = 0;
            long step = 0;
            if (splitCommand.size() >= 8) {
                freq = splitCommand.at(2).toLongLong();
                type = splitCommand.at(3).toShort();
                temper = splitCommand.at(4).toShort();
                DACBegin = splitCommand.at(5).toLong();
                pointsNumber = splitCommand.at(6).toLong();
                step = splitCommand.at(7).toLong();
            }
            char * commandBody = new char[24];
            short *PIDPtr = (short *)&commandBody[0];
            * PIDPtr = 0x26;
            long long * freqPtr = (long long *)&commandBody[2];
            * freqPtr = freq;
            short *typePtr = (short *)&commandBody[8];
            * typePtr = type;
            short *tempPtr = (short *)&commandBody[10];
            * tempPtr = temper;
            long *DACBeginPtr = (long *)&commandBody[12];
            * DACBeginPtr = DACBegin;
            long *pointsNumberPtr = (long *)&commandBody[16];
            * pointsNumberPtr = pointsNumber;
            long *stepPtr = (long *)&commandBody[20];
            * stepPtr = step;
            // заголовок команды
            sCommandHead head;
            head.Signature =  0x55AA;///< Признак начала команды = 0x55AA
            head.Command = 0x04 ;   ///< Код команды GET
            head.Status = -32768;    ///< Статус команды (MI_CRITICAL_ERROR)
            head.BodyLen = 24;   ///< Длина тела команды
            head.FlagA = 0x03;     ///< Флаг A
            head.FlagB = 0x20;     ///< Флаг B
            head.dummy = 0;     ///< Зарезервированное поле
            head.CommandID = 8; ///< Идентификатор команды
            int BytesToWrite = head.BodyLen + 16;
            char *command = new char[BytesToWrite];
            memcpy(&command[0], &head, 16);
            memcpy(&command[16], &commandBody[0], head.BodyLen);
            if(CheckErr(ptrTr->SendData(command,&BytesToWrite))) {
                return -1;  // отправка всего
            }
            ptrTr->SetSuppressMICS(VI_FALSE);
            delete [] command;
            delete [] commandBody;

            if (ReadAnsw) {
                Recv();
                sCommandHead *h;
//                char *buf = new char[65535];
                char *buf = GetRawData();
                h = reinterpret_cast<sCommandHead*>((char*)buf);
                int cnt = h->BodyLen;
                QString strres = "";
                long *data;
                short *temp;
                temp = (short*)&buf[16 + 10];
                strres = QString::number(*temp/256.0) + "\t";
                for (int i=16+24; i < cnt + 16 - 6; i = i + 4) {
                    data = (long*)&buf[i];
                    strres += QString::number(*data/256.0) + "\t";
                }
                strres = strres.left(strres.size() - 1);
                strres.simplified();
                SetAnsw(strres.toStdString());
                int err = *(short*)&buf[cnt + 16 - 2];
                if (CheckErr(err)) {
                    return -1;
                }

            }
            return 0;
        } else if (comm.contains("TEST 1")
                  || comm.contains("TEST 2")
                  || comm.contains("TEST 3")
                  || comm.contains("TEST 4")) {// анализ команды (команда TEST с параметром)
            int testID = -1;
            int bLen = -1;
            char * commandBody = 0;
            if (comm.contains("TEST 1")) {
                testID = 1;
                bLen = 6;
                commandBody = new char[bLen];
                short *p = (short*)&commandBody[0];
                *p = 0x0001;
                p = (short*)&commandBody[2];
                *p = 0x0000;
                p = (short*)&commandBody[4];
                *p = 0x8000;
            } else if (comm.contains("TEST 2")) {
                testID = 2;
                bLen = 2;
                commandBody = new char[bLen];
                short *p = (short*)&commandBody[0];
                *p = 0x0002;
            } else if (comm.contains("TEST 3")) {
                testID = 3;
                bLen = 4;
                commandBody = new char[bLen];
                short *p = (short*)&commandBody[0];
                *p = 0x0003;
                p = (short*)&commandBody[2];
                *p = 0x1000;
            } else if (comm.contains("TEST 4")) {
                testID = 4;
                bLen = 2;
                commandBody = new char[bLen];
                short *p = (short*)&commandBody[0];
                *p = 0x0004;
            }
            if (testID != -1) {
                sCommandHead head;
                head.Signature =  0x55AA;///< Признак начала команды = 0x55AA
                head.Command = 0x00 ;   ///< Код команды TEST
                head.Status = -32768;    ///< Статус команды (MI_CRITICAL_ERROR)
                head.BodyLen = bLen;   ///< Длина тела команды
                head.FlagA = 0x1;     ///< Флаг A
                head.FlagB = 0x20;     ///< Флаг B
                head.dummy = 0;     ///< Зарезервированное поле
                head.CommandID = 8; ///< Идентификатор команды



                int BytesToWrite = head.BodyLen + 16;

                if (testID == 1) { // если используется тест 1 (многократный посыл данных)
//                    char *buf = new char[65535];
                    sCommandHead *h;
                    for (int i = 0; i < 10; ++i) {

                        char *command = new char[BytesToWrite];
                        memcpy(&command[0], &head, 16);
                        memcpy(&command[16], &commandBody[0], head.BodyLen);
                        ptrTr->SetSuppressMICS(VI_TRUE);
                        if(CheckErr(ptrTr->SendData(command,&BytesToWrite))) {
//                            qDebug() << "Send: fuckup";
//                            qDebug() << "</Send()>";
                            return -1;  // отправка всего
                        }
                        ptrTr->SetSuppressMICS(VI_FALSE);
                        Recv(); // получение ответа
                        char *buf = GetRawData();
                        h = reinterpret_cast<sCommandHead*>((char*)buf);
                        int cnt = h->BodyLen;
                        QString strres = "";
                        char *data = new char[cnt - 2];
                        data = &buf[16 + 2];
                        strres.append(QByteArray(data,cnt - 2));
                        SetAnsw(strres.toStdString());
                        if (strres != QString("OK")) {
                            SetAnsw(QString("Error").toStdString());
                            break;
                        }
                        delete [] data;
                        delete [] command;
                    }
                    delete [] commandBody;
//                    delete [] buf;
                    return 0;
                } else {
                    char *command = new char[BytesToWrite];
                    memcpy(&command[0], &head, 16);
                    memcpy(&command[16], &commandBody[0], head.BodyLen);
                    ptrTr->SetSuppressMICS(VI_TRUE);
                    if(CheckErr(ptrTr->SendData(command,&BytesToWrite))) {
                        return -1;  // отправка всего
                    }
                    ptrTr->SetSuppressMICS(VI_FALSE);

                    delete [] commandBody;
                    delete [] command;

                    Recv(); // получение ответа
                    sCommandHead *h;
//                    char *buf = new char[65535];
                    char *buf = GetRawData();
                    h = reinterpret_cast<sCommandHead*>((char*)buf);
                    int cnt = h->BodyLen;
                    QString strres = "";
                    char *data = new char[cnt - 2];
                    if (testID == 3) {
                        if (cnt == 0xFF0) // отправляли 4096 - получили 4080, значит ок.
                            SetAnsw(QString("OK").toStdString());
                        else
                            SetAnsw(QString("Error").toStdString());
                    } else {
                        data = &buf[16 + 2];
                        strres.append(QByteArray(data,cnt - 2));
                    }
                    SetAnsw(strres.toStdString());
//                    delete [] buf;
                    delete [] data;
                    //            int err = *(short*)&buf[cnt + 16 - 2];

                    //            if (CheckErr(err))
                    //                return -1;
                    return 0;
                }

            } else {
                return 0;
            }
        } else {
            if (ReadAnsw && comm.contains("RUN")) {
                int errorCode = 1;
                QTime time;
                time.start();
                errorCode = ptrTr->SendData(comm.toLocal8Bit().data(),&cnt);
                if (CheckErr(errorCode)) {
                    sweepTime_ = time.elapsed();
                    return -1;
                }
                if (Recv()) {
                    sweepTime_ = time.elapsed();
                    return -1;
                }
                sweepTime_ = time.elapsed();
                return 0;

            } else {
                if (CheckErr(ptrTr->SendData(comm.toLocal8Bit().data(),&cnt))) {
                    return -1;
                }
            }
        }
        if((comm.contains("?")&& ReadAnsw && byByte)
                || (byByte && ReadAnsw))
        {
            QString answer = "";
            while (Answ[0] != termChar_) {
                if(Recv(false,1)) {
                    return -1;
                }
                answer.append(Answ[0]);
            }
            Answ = answer.toStdString();
        }
        else if(comm.contains("?")&& ReadAnsw)
        {
            if(Recv(true, 0)) {
                return -1;
            }
        }
        else if(isMICS && ReadAnsw)
        {
            if(Recv()) {
                return -1;
            }
        }
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::SendBuf(char* buf,int cnt)
{
    if(ptrTr)  {
        if(CheckErr(ptrTr->SendData(buf,&cnt))) {
            return -1;
        }
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::SetTimeOut(int TMO)
{
    if(ptrTr)  {
        if(CheckErr(ptrTr->SetTimeOut(TMO))) {
            return -1;
        }
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}


int TranspFuncs::Recv(bool transl,int nbyte)
{
    if (ptrTr) {
        qDebug()<<moduleLabel<<"Recv: " << "transl=" << transl;
//        ptrTr->SetTimeOut(45000);
        int sizeInBuff = 65535;
        int cnt = 65535;
        sCommandHead *head;

        if (isMICS && !transl) {
            sizeInBuff = 100;
            cnt = (nbyte>0) ? nbyte : 16;
            ptrTr->SetSuppressMICS(VI_TRUE);
            qDebug()<<"\t isMICS: " << "header size=" << cnt;
//            Sleep(100);
        }
        char *buf = new char[sizeInBuff];
        if ((nbyte > 0) && !isMICS) {
            cnt = nbyte;
        }
        if(CheckErr(ptrTr->RecvData(buf,&cnt))) {
            return -1;

        }
        qDebug()<<"\t Header recived ";
        buf[cnt] = '\0';
        Answ = buf;
        QString s;

        memcpy(RawData,buf,cnt);
        if (isMICS & !transl & !nbyte) {
            int start = cnt;
            head = reinterpret_cast<sCommandHead*>(buf);
            cnt = head->BodyLen;
            qDebug()<<"\t BodyLen=" << cnt;
            int ret = CheckErr(head->Status);
            int gcnt = cnt;
            char bufData[65535];
            while (cnt>0) {
                if(CheckErr(ptrTr->RecvData(bufData,&cnt))) {
                    qDebug()<<"\t Body NOT recived ";
                    delete [] buf;
                    return -1;
                }
                qDebug()<<"\t Body recived ";
                gcnt = gcnt - cnt;
                memcpy(&RawData[start],bufData,cnt);
                start = start+cnt;
                cnt = gcnt;
            }
            ptrTr->SetSuppressMICS(VI_FALSE);
            if(ret) {
                delete [] buf;
                return -1;
            }
        }
        delete [] buf;
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::Disconnect()
{
    if (isAddrDbl) {
        return 0;
    }
    if (ptrTr) {
        q_check_ptr(ptrTr);
        ptrTr->Disconnect();
        q_check_ptr(ptrTr);
        pDeleteTr(ptrTr);
        q_check_ptr(ptrTr);
        ptrTr = NULL;
    } else {
        return -1;
    }
    return 0;
}

void TranspFuncs::SetAnsw(string val)
{
    Answ = val;
}

string TranspFuncs::GetAnsw()
{
    string AnswTemp = Answ;
    Answ = "";
    return AnswTemp;
}
char* TranspFuncs::GetRawData()
{
    return RawData;
}

int TranspFuncs::CheckErr(int errCode)
{
    if(errCode)
    {
        qDebug()<<"CheckErr: errCode: "<<errCode;
        char errDescr[200];
        if(!ptrTr->GetErrorDesc(errCode,errDescr) && QString(errDescr)!="")
        {
            qDebug()<<"Error description: "<<errDescr;
            SetError(QString::fromLocal8Bit(errDescr));
        }
        else
        {
            switch(errCode)
            {
            case 1:
                SetError(tr("Parameter corrected"));
                break;
            case 2:
                SetError(tr("Parameter ignored"));
                break;
            case 3:
                SetError(tr("Measurement timeout"));
                break;
            case 4:
                SetError(tr("Ethernet connection is closed"));
                break;
            case 5:
                SetError(tr("Operation complete"));
                break;
            case 6:
                SetError(tr("Source power unlevel"));
                break;
            case 7:
                SetError(tr("Data flow is broken"));
                break;
            case 8:
                SetError(tr("RF overload"));
                break;
            case 9:
                SetError(tr("ADC overload"));
                break;
            case 10:
                SetError(tr("IF overload"));
                break;
            case 11:
                SetError(tr("IF1 overload"));
                break;
            case 12:
                SetError(tr("IF2 overload"));
                break;
            case 13:
                SetError(tr("IF3 overload"));
                break;
            case 14:
                SetError(tr("IF4 overload"));
                break;
            case 15:
                SetError(tr("IF5 overload"));
                break;
            case 16:
                SetError(tr("XML value is empty"));
                break;
            case 17:
                SetError(tr("Specified power is not reached"));
                break;
            case 18:
                SetError(tr("Data flow is logicaly broken"));
                break;
            case 19:
                SetError(tr("Measurement is not valid"));
                break;
            case 20:
                SetError(tr("Not enough frequency resolution"));
                break;
            case 21:
                SetError(tr("Operation not complete"));
                break;
            case 22:
                SetError(tr("External reference oscillator unlock"));
                break;
            case 23:
                SetError(tr("External reference oscillator no power"));
                break;
            case 20486:
                SetError(tr("Read timeout"));
                break;
            case 20487:
                SetError(tr("Write timeout"));
                break;
            case 20503:
                SetError(tr("Unknown software error"));
                break;
            case 20505:
                SetError(tr("Operation canceled"));
                break;
            case 20506:
                SetError(tr("Connection lost"));
                break;
            case -32768:
                SetError(tr("Device is in critical state"));
                break;
            case -32767:
                SetError(tr("Unknown command"));
                break;
            case -32766:
                SetError(tr("Unknown parameter"));
                break;
            case -32765:
                SetError(tr("Incorrect parameter"));
                break;
            case -32764:
                SetError(tr("No parameters"));
                break;
            case -32763:
                SetError(tr("Insufficient command size"));
                break;
            case -32762:
                SetError(tr("PLD error"));
                break;
            case -32761:
                SetError(tr("LO1 no PLL"));
                break;
            case -32760:
                SetError(tr("LO2 no PLL"));
                break;
            case -32759:
                SetError(tr("LO3 no PLL"));
                break;
            case -32758:
                SetError(tr("Digital synchronization timeout"));
                break;
            case -32757:
                SetError(tr("Synthesizer timeout"));
                break;
            case -32756:
                SetError(tr("ADC error"));
                break;
            case -32755:
                SetError(tr("Invalid command size"));
                break;
            case -32754:
                SetError(tr("Invalid signature. Command thread is failed."));
                break;
            case -32753:
                SetError(tr("EPROM error"));
                break;
            case -32752:
                SetError(tr("EPROM mismatch"));
                break;
            case -32751:
                SetError(tr("Incorrect index table of regul curve"));
                break;
            case -32750:
                SetError(tr("Incorrect size of regul curve"));
                break;
            case -32749:
                SetError(tr("LVDS timeout"));
                break;
            case -32748:
                SetError(tr("LVDS device missing"));
                break;
            case -32747:
                SetError(tr("LVDS device register missing"));
                break;
            case -32746:
                SetError(tr("Analog asynchronous timeout"));
                break;
            case -32745:
                SetError(tr("Remote control denied"));
                break;
            case -32744:
                SetError(tr("Invalid file format"));
                break;
            case -32743:
                SetError(tr("LVDS packets duplication"));
                break;
            case -32742:
                SetError(tr("Synthesizer mode error"));
                break;
            case -32741:
                SetError(tr("SATA error"));
                break;
            case -32740:
                SetError(tr("External reference oscillator no PLL"));
                break;
            case -32739:
                SetError(tr("Synthesizer no PLL"));
                break;
            case -32738:
                SetError(tr("LO4 no PLL"));
                break;
            case -32737:
                SetError(tr("LO5 no PLL"));
                break;
            case -32736:
                SetError(tr("External reference oscillator no PLL during measument"));
                break;
            case -32735:
                SetError(tr("External reference oscillator no power"));
                break;
            case -32734:
                SetError(tr("External reference oscillator no power during measument"));
                break;
            case -32733:
                SetError(tr("Reference oscillator fault"));
                break;
            default:
                SetError(tr("Unknown error ")+QString::number(errCode));
            }
        }
        if(errCode<0) {
            return -1;
        }
    }
    return 0;
}

int TranspFuncs::SleepMS(int mstime)
{
    Sleep(mstime);
    return 0;
}

int TranspFuncs::SetModuleLabel(string name)
{
    moduleLabel = QString::fromStdString(name);
    return 0;
}

