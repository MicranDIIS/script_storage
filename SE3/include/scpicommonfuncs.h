#ifndef SCPICOMMONFUNCS_H
#define SCPICOMMONFUNCS_H

#include "../include/ITranspFuncs.h"
#include <../include/Transport.h>
#include "Libs_global.h"
#include "LuaRef.h"
#include "errormanager.h"

typedef ICTransport* (*ptr_createTr)(QString,bool&);
typedef void (*ptr_deleteTr)(ICTransport*);

static ptr_createTr spCrTr;
static ptr_deleteTr spDelTr;
static lua_State* ls;

class ScpiCommonFuncs : public QObject, public ITranspFuncs, public ErrorManager
{
public:
    ScpiCommonFuncs();
    ~ScpiCommonFuncs();
    int Connect(std::string addr);
    int GetMeasTime();
    int Send(std::string str_comm, bool ReadAnsw = true);
    int Disconnect();
    int SleepMS(int mstime);
    int SetTermChar(string termChar);
    int SetVisaType(int type);
    std::string GetAnsw();
    int SetTimeout(int msec);    
protected:
    int Recv(bool transl = false);
    int CheckErr(int errCode);
    void SetAnsw(std::string val);

    ICTransport* ptrTr;    
    ptr_createTr pCreateTr;
    ptr_deleteTr pDeleteTr;
    string Answ;
    QString DevAddr;
    int visaType_;
    bool isAddrDbl;
    QString termChar_;
    int timeOut_;
    int measTime_;
};

extern "C" LIB_DEST void RegTrFuncs(ptr_createTr,ptr_deleteTr);
#endif // SCPICOMMONFUNCS_H
