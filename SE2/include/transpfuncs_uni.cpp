#include "transpfuncs_uni.h"
#include "sstream"

static ptr_createTr spCrTr;
static ptr_deleteTr spDelTr;

void RegTrFuncs(ptr_createTr pCrTr, ptr_deleteTr pDelTr)
{
    qDebug()<<"TrFunc: RegTrFunc: enter";
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
    timeout_ = 45000;
}

TranspFuncs::~TranspFuncs()
{
    qDebug()<<"TrFunc: Destructor: enter";
    if(ptrTr&&pDeleteTr){
        qDebug()<<"TrFunc: Destructor: success";
        pDeleteTr(ptrTr);
        ptrTr = NULL;
    }
}

int TranspFuncs::Connect(string addr, int LibInit, string termCh)
{
    qDebug()<<"TrFunc: Connect: start (params: addr="<<QString::fromStdString(addr)<<",LibInit="<<LibInit;
    int bin = 1;
    QString VAddr=QString::fromStdString(addr);
    if(VAddr.contains("::8888::")&&!LibInit)
    {
        VAddr+="::MICS";
        isMICS = true;
    }
    else
        bin = 0;
    qDebug()<<VAddr;
    ptrTr = pCreateTr(VAddr,isAddrDbl);
    if(ptrTr)
    {   
        if(isAddrDbl) return 0;
        int code = ptrTr->InitClass(LibInit);
        if(CheckErr(code)) {
            qDebug() << "TrFunc: Connect(): InitClass(): error. Code =" << QString::number(code) ;
            return -1;
        }
        char termChar = termCh[0];
        termChar_ = termChar;
        code = ptrTr->Connect(VAddr.toAscii().data(),bin);
        if(CheckErr(code)) {
            qDebug() << "TrFunc: Connect(): Connect(): error. Code =" << QString::number(code) ;
            return -1;
        }
    }
    else
    {
        qDebug()<<"TrFunc: Connect: pointer error";
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    qDebug() << "return 0";
    return 0;
}

int TranspFuncs::Send(string str_comm,bool ReadAnsw, bool byByte)
{
    qDebug()<<"TrFunc: Send: start (params: str_comm=" << QString::fromStdString(str_comm) << ",ReadAnsw=" << ReadAnsw;
    if(ptrTr)
    {
        QString comm = QString::fromStdString(str_comm);
        if(!comm.contains("\n"))
            comm+="\n";
        int cnt = comm.count();

        qDebug()<<QString("TrFunc: Send: ready for sending: \"%1\"").arg(comm);
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
//            qDebug() << DACBegin;
//            qDebug() << pointsNumber;
//            qDebug() << step;
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
            int BytesToWrite;

            BytesToWrite = 16;
            if(CheckErr(ptrTr->SendData(&head,&BytesToWrite))) return -1;   // отправка заголовка
            BytesToWrite = head.BodyLen;
            if(CheckErr(ptrTr->SendData(commandBody,&BytesToWrite))) return -1;   // отправка тела

            ptrTr->SetSuppressMICS(VI_FALSE);
            Recv();


            sCommandHead *h;
            char *buf = new char[65535];
            buf = GetRawData();
            h = reinterpret_cast<sCommandHead*>((char*)buf);
            int cnt = h->BodyLen;



            QString strres = "";
            long *data;
//            if (1) {

                short *temp;


                temp = (short*)&buf[16 + 10];
                strres = QString::number(*temp/256.0) + "\t";
//                qDebug() << "temp = " << strres;

                for (int i=16+24; i < cnt + 16 - 6; i = i + 4) {
                    data = (long*)&buf[i];
                    strres += QString::number(*data/256.0) + "\t";                    
//                    qDebug() << "i = " << i;
                }
                strres = strres.left(strres.size() - 1);
                strres.simplified();
                SetAnsw(strres.toStdString());
//                qDebug() << strres;
//            } else {
//                long *data;
//                data = ((long*)&buf[16]);
//                SetAnsw(QString::number(*data).toStdString());
//            }
            int err = *(short*)&buf[cnt + 16 - 2];;
            delete [] buf;
            if(CheckErr(err)) return -1;
            return 0;
        } else {
            if(CheckErr(ptrTr->SendData(comm.toAscii().data(),&cnt))) {
                return -1;
            }
        }
        qDebug()<<QString("TrFunc: Send: Sending %1 is sucsessfull").arg(comm);
//        qDebug()<< "TrFunc: Send: Sending is sucsessfull";
        if((comm.contains("?")&& ReadAnsw && byByte)
                || (byByte && ReadAnsw))
        {
            QString answer = "";
            while (Answ[0] != termChar_) {
                if(Recv(false,1))
                    return -1;
                answer.append(Answ[0]);
            }
            Answ = answer.toStdString();
        }
        else if(comm.contains("?")&& ReadAnsw)
        {
            if(Recv(true, 0))
                return -1;
        }
        else if(isMICS && ReadAnsw)
        {
            qDebug() << isMICS;
            if(Recv())
                return -1;
        }
    }
    else
    {   qDebug()<<"TrFunc: Send: pointer error";
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::SendBuf(char* buf,int cnt)
{
    qDebug()<<"TrFunc: SendBuf: start";    if(ptrTr)
    {
//        if(!isMICS)
//            ptrTr->SetSuppressMICS(VI_TRUE);
        if(CheckErr(ptrTr->SendData(buf,&cnt))) return -1;
//        if(!isMICS)
//            ptrTr->SetSuppressMICS(VI_FALSE);
    }
    else
    {
        qDebug()<<"TrFunc: SendBuf: pointer error";
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::SetTimeout(int mseconds)
{
    timeout_ = mseconds;
    return 0;
}

int TranspFuncs::Recv(bool transl,int nbyte)
{
    qDebug()<<"TrFunc: Recv: start";
    if(ptrTr)
    {
        ptrTr->SetTimeOut(timeout_);
        int sizeInBuff = 65535;
        int cnt = 65535;
        sCommandHead *head;

        if(isMICS && !transl)
        {
            sizeInBuff = 100;
            cnt = (nbyte>0) ? nbyte : 16;
            ptrTr->SetSuppressMICS(VI_TRUE);
            Sleep(100);
        }
        char *buf = new char[sizeInBuff];        
        if ((nbyte > 0) && !isMICS) {
            cnt = nbyte;
        }
        qDebug()<< cnt;
        if(CheckErr(ptrTr->RecvData(buf,&cnt))) {

            return -1;

          }

        buf[cnt] = '\0';
        Answ = buf;
        QString s;

//        qDebug()<< s.fromStdString(Answ);

        memcpy(RawData,buf,cnt);
        if(isMICS & !transl & !nbyte)
        {
            int start = cnt;
            qDebug()<<"TrFunc: Recv: Read head";
            head = reinterpret_cast<sCommandHead*>(buf);
            cnt = head->BodyLen;
//            qDebug()<<QString("Recieving head: %1").arg(cnt);
            int ret = CheckErr(head->Status);
            int gcnt = cnt;
            char bufData[65535];
            while(cnt>0)
            {
                qDebug()<<QString("TrFunc: Recv: Recieving...");
                if(CheckErr(ptrTr->RecvData(bufData,&cnt))) {
                    return -1;

                }
                gcnt = gcnt - cnt;
                qDebug()<<QString("TrFunc: Recv: Recieving: %1").arg(QString(bufData));
                qDebug()<<QString("TrFunc: Recv: Recieving byte: %1").arg(cnt);
                memcpy(&RawData[start],bufData,cnt);
                start = start+cnt;
                cnt = gcnt;
            }

            ptrTr->SetSuppressMICS(VI_FALSE);
            if(ret) {
                return -1;
            }
        }

        delete [] buf;
    }
    else
    {
        qDebug()<<"TrFunc: Recv: pointer error";
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int TranspFuncs::Disconnect()
{
    qDebug()<<"TrFunc: Disconnect: start";
    if(isAddrDbl) return 0;
    if(ptrTr)
    {
        if(CheckErr(ptrTr->Disconnect())) return -1;
        pDeleteTr(ptrTr);
        ptrTr = NULL;
    }
    else
    {
        qDebug()<<"TrFunc: Disconnect: pointer error";
        SetError(tr("Do not create a transport class"));
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
    qDebug()<<"TrFunc: CheckErr: start, param: errCode:"<<errCode;
    if(errCode)
    {
        char errDescr[200];
        if(!ptrTr->GetErrorDesc(errCode,errDescr) && QString(errDescr)!="")
        {
            SetError(QString::fromAscii(errDescr));
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
        if(errCode<0) return -1;
    }
    return 0;
}

int TranspFuncs::SleepMS(int mstime)
{
    Sleep(mstime);
    return 0;
}

int TranspFuncs::SetAttribute(double attr, double val)
{
    if(ptrTr)
    {
	if(CheckErr(ptrTr->SetAttribute(attr,val)))
		return -1;
    }
    else{
	qDebug()<<"TrFunc: SetError: pointer error";
	SetError(tr("Do not create a transport class"));
	return -1;
    }
    return 0;
}

int TranspFuncs::GetAttribute(double attr)
{
    if(ptrTr)
    {
	double val;
	if(CheckErr(ptrTr->GetAttribute(attr,&val)))
		return -1;
	std::ostringstream sstream;
	uint32_t vali = static_cast<uint32_t>(val);
	sstream << vali;
	SetAnsw(sstream.str());
    }
    else{
	qDebug()<<"TrFunc: SetError: pointer error";
	SetError(tr("Do not create a transport class"));
	return -1;
    }
    return 0;
}
