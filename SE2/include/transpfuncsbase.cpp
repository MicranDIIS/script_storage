#include "transpfuncsbase.h"
#include "ctype.h"

static createTransportType spCreateTransport;
static deleteTransportType spDeleteTransport;
//static lua_State* pLuaState;

CTranspFuncsBase::CTranspFuncsBase()
{
    m_pTransport = 0;
    m_bIsAddrDbl = false;
    pCreateTr = spCreateTransport;
    pDeleteTr = spDeleteTransport;
    m_nTimeout = 45000;
    m_szRawData = new char[BUFF_SIZE];
     qDebug()<<"Transp construct"<<m_pTransport;
}

CTranspFuncsBase::~CTranspFuncsBase()
{
    if (m_pTransport&&pDeleteTr) {
        pDeleteTr(m_pTransport);
        m_pTransport = NULL;
    }
    delete m_szRawData;
}

int CTranspFuncsBase::Connect(string sAddr, string sTermCh)
{
    QString sVisaAddr=QString::fromStdString(sAddr);

    sVisaAddr = CorrectAddr(sVisaAddr);

    m_pTransport = pCreateTr();
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

        m_cTermChar = sTermCh[0];
	qDebug()<<"set tc";
	if (CheckErr(m_pTransport->Connect(sVisaAddr.toLocal8Bit().data(),(sTermCh.empty()||sTermCh[0]==' ') ? 1 : 0))) {
            pDeleteTr(m_pTransport);
            m_pTransport = NULL;
            return -1;
        }
	qDebug()<<"set tc res"<<m_cTermChar;
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

string CTranspFuncsBase::GetAnsw()
{
    string sAnswTemp = m_sAnsw;
    m_sAnsw = "";
    return sAnswTemp;
}

char* CTranspFuncsBase::GetRawData()
{
    return m_szRawData;
}

int CTranspFuncsBase::SetRawData(int nStartIndex, char* szBuf, int nSize)
{
    if(nSize>BUFF_SIZE)
    {
        SetError(tr("SetRawData: size for copy is too large"));
        return -1;
    }
    if( (nStartIndex + nSize) > BUFF_SIZE)
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
    qDebug()<<nErrCode;
    if(nErrCode)
    {
        char szErrDescr[200];
        if(m_pTransport && !(m_pTransport->GetErrorDesc(nErrCode,szErrDescr) )&& QString(szErrDescr)!="")
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
    spCreateTransport = pCreateTransport;
    spDeleteTransport = pDeleteTransport;
}

char CTranspFuncsBase::GetTermChar()
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

int CTranspFuncsBase::SetAttribute(double attr, double val)
{
    if(m_pTransport)  {
	if(CheckErr(m_pTransport->SetAttribute(attr, val))) {
	    return -1;
	}
    } else {
	SetError(tr("Do not create a transport class"));
	return -1;
    }
    return 0;
}

int CTranspFuncsBase::GetAttribute(double attr)
{
    if(m_pTransport)  {
	double val;
	if(CheckErr(m_pTransport->GetAttribute(attr, &val))) {
	    return -1;
	}
	SetAnsw(QString::number(val).toStdString());
    } else {
	SetError(tr("Do not create a transport class"));
	return -1;
    }
    return 0;
}



