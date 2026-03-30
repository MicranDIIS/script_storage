#include "transpfuncsbase.h"
#include "ctype.h"

#ifndef WIN32
    #include "unistd.h"
    void Sleep(unsigned int msec)
    {
        usleep(msec*1000);
    }
#else
#include "synchapi.h"
#endif

CTranspFuncsBase::CTranspFuncsBase()
{
    m_pTransport = 0;
    m_bIsAddrDbl = false;
//    pCreateTr = spCreateTransport;
//    pDeleteTr = spDeleteTransport;
    m_nTimeout = 45000;
     qDebug()<<"Transp construct"<<m_pTransport<<spCreateTransport<<spDeleteTransport;
}

CTranspFuncsBase::~CTranspFuncsBase()
{
    if (m_pTransport&&pDeleteTr) {
        pDeleteTr(m_pTransport);
        m_pTransport = NULL;
    }
}

int CTranspFuncsBase::Connect(char* sAddr, const char* sTermCh)
{
    qDebug()<<"SCPI connect"<<QThread::currentThread();
    pCreateTr = spCreateTransport;
    pDeleteTr = spDeleteTransport;
    QString sVisaAddr=QString::fromLocal8Bit(sAddr);

    sVisaAddr = CorrectAddr(sVisaAddr);
    qDebug()<<"connecting"<<sVisaAddr<<pCreateTr;

    m_pTransport = pCreateTr(sVisaAddr,m_bIsAddrDbl);
    qDebug()<<"create tr"<<m_pTransport<<m_bIsAddrDbl;
    if (m_pTransport) {
        if (m_bIsAddrDbl) {
            return 0;
        }
	qDebug()<<"init tr";
        if (CheckErr(m_pTransport->InitClass(GetTransportType()))) {
            pDeleteTr(m_pTransport);
            m_pTransport = NULL;
            return -1;
        }
        m_cTermChar = (string)sTermCh/*[0]*/;
	qDebug()<<"set tc";
    if (CheckErr(m_pTransport->Connect(sVisaAddr.toLocal8Bit().data(),(m_cTermChar.empty()||m_cTermChar[0]==' ') ? 1 : 0))) {
            pDeleteTr(m_pTransport);
            m_pTransport = NULL;
            return -1;
        }
//	qDebug()<<"set tc res"<<m_cTermChar;
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int CTranspFuncsBase::SendBuf(char* szBuf, int nCnt)
{
    if(m_pTransport)  {        
        if(CheckErr(m_pTransport->SendData(szBuf,&nCnt))) {
            return -1;
        }
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int CTranspFuncsBase::RecvBuf(char *szBuf, int *pnCnt)
{
    if(m_pTransport)  {
        if(CheckErr(m_pTransport->RecvData(szBuf, pnCnt))) {
            return -1;
        }
    }
    else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int CTranspFuncsBase::SetTimeout(int nMSeconds)
{
    if(m_pTransport)  {
        m_pTransport->SetTimeOut(nMSeconds);
        m_nTimeout = nMSeconds;
    }
    else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int CTranspFuncsBase::SetSuppressMics(bool bOn)
{
    if(m_pTransport)  {
        m_pTransport->SetSuppressMICS(bOn);
    }
    else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int CTranspFuncsBase::Disconnect()
{
    if (m_bIsAddrDbl) {
        return 0;
    }
    if (m_pTransport) {
        m_pTransport->Disconnect();
        pDeleteTr(m_pTransport);
        m_pTransport = NULL;
    } else {
        return -1;
    }
    return 0;
}

const char* CTranspFuncsBase::GetAnsw()
{
    m_tempAnsw = m_sAnsw;
    m_sAnsw = "";
    return m_tempAnsw.data();
}

char* CTranspFuncsBase::GetRawData()
{

    return m_szRawData;
}

int CTranspFuncsBase::SetRawData(int nStartIndex, char* szBuf, int nSize)
{
    if(nSize>65535)
    {
        SetError(tr("SetRawData: size for copy is too large"));
        return -1;
    }
    if( (nStartIndex + nSize) > 65535)
    {
        SetError(tr("SetRawData: Copy will overflow buffer"));
        return -1;
    }
    memcpy(&m_szRawData[nStartIndex],szBuf,nSize);
    return 0;
}

void CTranspFuncsBase::SetAnsw(string sVal)
{
    m_sAnsw = sVal;
}

int CTranspFuncsBase::SleepMS(int nMsTime)
{
    Sleep(nMsTime);
    return 0;
}

int CTranspFuncsBase::CheckErr(int nErrCode)
{

    if(nErrCode<0)
    {
        char szErrDescr[200];
        ViStatus st = m_pTransport->GetErrorDesc(nErrCode,szErrDescr);
        qDebug()<<nErrCode<<";"<<st<<";"<<szErrDescr;
        if(m_pTransport && !(m_pTransport->GetErrorDesc(nErrCode,szErrDescr) )&& !QString(szErrDescr).isEmpty())
        {
            SetError(QString::fromLocal8Bit(szErrDescr));
        }
        else
        {
            CheckSpecificErr(nErrCode);
        }
        if(nErrCode<0) {
            return -1;
        }
    }
    return 0;
}

void RegTrFuncs(createTransportType pCreateTransport, deleteTransportType pDeleteTransport)
{
    qDebug()<<"registrate tr funcs"<<pCreateTransport<<pDeleteTransport;
    spCreateTransport = pCreateTransport;
    spDeleteTransport = pDeleteTransport;
}

string CTranspFuncsBase::GetTermChar()
{
    return m_cTermChar;
}

int CTranspFuncsBase::GetTransportType()
{
    return m_nTransportType;
}

void CTranspFuncsBase::SetTransportType(int nTrType)
{
    m_nTransportType = nTrType;
}




