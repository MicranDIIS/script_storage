#include "transpfuncsscpi.h"

CTranspFuncsScpi::CTranspFuncsScpi()
{
    SetTransportType(AGILENT_LIB);
}

int CTranspFuncsScpi::Send(char* sComm)
{
    QString sCommand = QString::fromLocal8Bit(sComm);
    QString tchar = QString::fromStdString(GetTermChar());
    if(!sCommand.contains(/*"\n"*/tchar))
        sCommand+=tchar/*"\n"*/;
        int nCnt = sCommand.count();

	qDebug()<<"send"<<sCommand;
        if (SendBuf(sCommand.toLocal8Bit().data(),nCnt))
            return -1;

        if(sCommand.contains("?"))
        {
            if(Recv())
                return -1;
        }
    return 0;
}

int CTranspFuncsScpi::Recv()
{
    int nSizeInBuff = 65535;
    int nCnt = 65535;

    char *szBuf = new char[nSizeInBuff];
    qDebug()<<"start recv";
    if(RecvBuf(szBuf,&nCnt)) {
        return -1;
    }

    szBuf[nCnt] = '\0';
    SetAnsw(szBuf);
    delete [] szBuf;
    return 0;
}

QString CTranspFuncsScpi::CorrectAddr(QString sAddr)
{
    return sAddr;
}

void CTranspFuncsScpi::CheckSpecificErr(int nErrCode)
{
    SetError(tr("Unknown error ")+QString::number(nErrCode));
}

int CTranspFuncsScpi::GetTransportType()
{
    return AGILENT_LIB;
}

int CTranspFuncsScpi::InitMiTransport(bool bState)
{
    if(bState)
        SetTransportType(MIVISA_LIB);
    else
        SetTransportType(AGILENT_LIB);
    return 0;
}
