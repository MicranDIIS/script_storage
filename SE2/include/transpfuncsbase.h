#ifndef TRANSPFUNCSBASE_H
#define TRANSPFUNCSBASE_H

#include <Transport.h>
#include <QString>
#include <QtCore>
#include "errormanager.h"
#include "Libs_global.h"
#include "LuaRef.h"

typedef ICTransport* (*createTransportType)();
typedef void (*deleteTransportType)(ICTransport*);
static lua_State* ls;

#define BUFF_SIZE 1024000

using namespace std;

enum{
    MIVISA_LIB = 0,
    AGILENT_LIB
};

class CTranspFuncsBase : public ErrorManager{
    Q_OBJECT

public:
    CTranspFuncsBase();
    ~CTranspFuncsBase();

    int Connect(string sAddr, string sTermCh = "\n");
    int SendBuf(char* szBuf, int nCnt);
    int RecvBuf(char* szBuf, int* pnCnt);
    int SetTimeout(int nMSeconds);
    int SetSuppressMics(bool bOn);
    int Disconnect();
    string GetAnsw();
    char* GetRawData();
    int SetRawData(int nStartIndex, char* szBuf, int nSize);
    void SetAnsw(string sVal);
    int SleepMS(int nMsTime);
    int CheckErr(int nErrCode) ;
    char GetTermChar();
    int GetTransportType();
    void SetTransportType(int nTrType);
    int SetAttribute( double attr, double val);
    int GetAttribute( double attr);

    virtual void CheckSpecificErr(int nErrCode) = 0;
    virtual QString CorrectAddr(QString sAddr) = 0;
    virtual int InitMiTransport(bool bState) = 0;

protected:
    ICTransport* m_pTransport;

private:
    bool m_bIsAddrDbl;
    string m_sAnsw;
    char *m_szRawData;//[BUFF_SIZE];
    QString m_sDevAddr;
    char m_cTermChar;
    int m_nTimeout;
    int m_nTransportType;

    createTransportType pCreateTr;
    deleteTransportType pDeleteTr;
};

extern "C" LIB_DEST void RegTrFuncs(createTransportType,deleteTransportType);

#endif // TRANSPFUNCSBASE_H
