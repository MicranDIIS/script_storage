#ifndef TRANSPFUNCSBASE_H
#define TRANSPFUNCSBASE_H

#include <../include/Transport.h>
#include <QString>
#include <QtCore>
#include "errormanager.h"
#include "Libs_global.h"
#include "LuaRef.h"

typedef ICTransport* (*createTransportType)(QString,bool&);
typedef void (*deleteTransportType)(ICTransport*);

static createTransportType spCreateTransport;
static deleteTransportType spDeleteTransport;
static void* pStateMachine;

using namespace std;

enum{
    MIVISA_LIB = 0,
    AGILENT_LIB
};

class CTranspFuncsBase : public QObject, public ErrorManager{
    Q_OBJECT

public:
    CTranspFuncsBase();
    ~CTranspFuncsBase();

    Q_INVOKABLE int Connect(char* sAddr, const char* sTermCh = "\n");
    int SendBuf(char* szBuf, int nCnt);
    int RecvBuf(char* szBuf, int* pnCnt);
    Q_INVOKABLE int SetTimeout(int nMSeconds);
    Q_INVOKABLE int SetSuppressMics(bool bOn);
    Q_INVOKABLE int Disconnect();
    Q_INVOKABLE const char* GetAnsw();
    Q_INVOKABLE char* GetRawData();
//    Q_INVOKABLE const char* GetError(){return ErrorManager::GetError();}
    int SetRawData(int nStartIndex, char* szBuf, int nSize);
    void SetAnsw(string sVal);
    int SleepMS(int nMsTime);
    int CheckErr(int nErrCode) ;
    string GetTermChar();
    int GetTransportType();
    void SetTransportType(int nTrType);

    virtual void CheckSpecificErr(int nErrCode) = 0;
    virtual QString CorrectAddr(QString sAddr) = 0;
    virtual int InitMiTransport(bool bState) = 0;

    createTransportType pCreateTr;
    deleteTransportType pDeleteTr;
    ICTransport* GetTransportPrt(){return m_pTransport;}

private:
    ICTransport* m_pTransport;
    bool m_bIsAddrDbl;
    string m_sAnsw;
    char m_szRawData[65535];
    QString m_sDevAddr;
    /*char*/string m_cTermChar;
    int m_nTimeout;
    int m_nTransportType;
    string m_tempAnsw;
};

extern "C" LIB_DEST void RegTrFuncs(createTransportType,deleteTransportType);

#endif // TRANSPFUNCSBASE_H
