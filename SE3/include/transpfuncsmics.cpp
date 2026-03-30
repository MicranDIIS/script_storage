#include "transpfuncsmics.h"

CTranspFuncsMics::CTranspFuncsMics():CTranspFuncsBase()
{
    SetTransportType(MIVISA_LIB);
}

QString CTranspFuncsMics::CorrectAddr(QString sAddr)
{
    if (sAddr.contains("::8888::")&&!(sAddr.contains)("::MICS")) {
        sAddr+="::MICS";
    }
    return sAddr;
}

int CTranspFuncsMics::Send(char* sComm, bool bReadAnsw, bool bReadByByte)
{
    QString sCommand = QString::fromLocal8Bit(sComm);
    if(!sCommand.contains("\n"))
        sCommand+="\n";
    int cnt = sCommand.count();

    if(SendBuf(sCommand.toLocal8Bit().data(),cnt))
        return -1;

    bool transl = false;

    if(bReadAnsw)
    {
        if(sCommand.contains("?"))
            transl = true;

        if(bReadByByte)
        {
            QString sAnswer = "";
            while (GetAnsw()[0] != GetTermChar().data()[0]) {
                if(Recv(false,1)) {
                    return -1;
                }
                sAnswer.append(GetAnsw()[0]);
            }
            SetAnsw(sAnswer.toStdString());
        }
        else
            if(Recv(transl)) {
                return -1;
            }
    }
    return 0;
}

int CTranspFuncsMics::Recv(bool bTransl, int nByte)
{
    int nSizeInBuff = bTransl ? 65535 : 100;
    int nCnt = !bTransl ? 16 : nByte>0 ? nByte : 65535;

    if (!bTransl)
        SetSuppressMics(true);

    char *szBuf = new char[nSizeInBuff];

    if(RecvBuf(szBuf,&nCnt))
        return -1;

    szBuf[nCnt] = '\0';
    SetAnsw(szBuf);
    SetRawData(0,szBuf,nCnt);

    if ( !bTransl & !nByte) {
        stCommandHead* pHead;
        int nStart = nCnt;
        pHead = reinterpret_cast<stCommandHead*>(szBuf);
        nCnt = pHead->BodyLen;
        int nErr = CheckErr( pHead->Status );
        int nGCnt = nCnt;
        char szBufData[65535];
        while ( nCnt>0 ) {
            if( RecvBuf(szBufData, &nCnt) ){
                delete [] szBuf;
                return -1;
            }
            nGCnt = nGCnt - nCnt;
            SetRawData(nStart, szBufData, nCnt);
            nStart = nStart+nCnt;
            nCnt = nGCnt;
        }
        SetSuppressMics(false);
        if ( nErr ) {
            delete [] szBuf;
            return -1;
        }
    }
    delete [] szBuf;
    return 0;
}

void CTranspFuncsMics::CheckSpecificErr(int nErrCode)
{
    switch(nErrCode)
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
        SetError(tr("Unknown error ")+QString::number(nErrCode));
    }
}


int CTranspFuncsMics::SetRegLVDS(int nAddress, int nReg, int nWrData, bool bRead)
{
    uniTunRegister packet;
    uniTunRegister *pPacket;
    packet.Frame=0x00;
    //Заполняем поля пакета
    if ( bRead ) packet.Frame=0x80000000;
    packet.Scheme.Address=nAddress;
    packet.Scheme.RegAddress=nReg;
    packet.Scheme.Data=nWrData;
    //Отправка пакета.псевдокод P_INT_COMMAND=packet.Frame
    if( CheckErr(Send(QString("P_INT_COMMAND = %1").arg((unsigned long)packet.Frame).toLocal8Bit().data())) )
        return -1;
    //Читаем ответ -вычитали пакет ответа.
    char *szBuf = GetRawData();
    pPacket=reinterpret_cast<uniTunRegister*>(&szBuf[18]);
    if (bRead){
        SetAnsw(QString::number(pPacket->Scheme.Data).toStdString());
    }
    if (!bRead && !pPacket->Scheme.Done) {
        return -32748;
    }
    return 0;
}

int CTranspFuncsMics::InitMiTransport(bool bState)
{
    int nState = bState ? MIVISA_LIB : AGILENT_LIB;
    SetTransportType(nState);
    return 0;
}
