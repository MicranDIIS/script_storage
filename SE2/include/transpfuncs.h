#ifndef TRANSPFUNCS_H
#define TRANSPFUNCS_H

#include <Transport.h>
#include <QString>
#include "errormanager.h"
#include <QtCore>
#include "Libs_global.h"
#include "LuaRef.h"

typedef ICTransport* (*ptr_createTr)(QString,bool&);
typedef void (*ptr_deleteTr)(ICTransport*);
static lua_State* ls;
using namespace std;

struct sCommandHead
{	unsigned short  Signature; ///< Признак начала команды = 0x55AA
        unsigned short  Command;   ///< Код команды
        short           Status;    ///< Статус команды
        unsigned short  BodyLen;   ///< Длина тела команды
        unsigned char   FlagA;     ///< Флаг A
        unsigned char   FlagB;     ///< Флаг B
        unsigned short	dummy;     ///< Зарезервированное поле
        unsigned long	CommandID; ///< Идентификатор команды
};

class TranspFuncs : public ErrorManager{
    Q_OBJECT
public:

    TranspFuncs();
    ~TranspFuncs();
    int Connect(string addr,int LibInit = 0, string termCh = "\n");
    int Send(string comm,bool ReadAnsw = true, bool byByte = false);
    int SendBuf(char* buf, int cnt);
    int SetBaudRate(int baud);
    int SetTimeout(int mseconds);
    int Recv(bool transl = false, int nbyte = 0);
    int Disconnect();
    string GetAnsw();
    char* GetRawData();
    int CheckErr(int errCode);
    void SetAnsw(string val);
    int SleepMS(int mstime);
    int SweepTime();
    int SetModuleLabel(string name);

private:
    bool isAddrDbl;
    bool isSCPI;
    string Answ;


    bool isMICS;
    char RawData[65535];
    QString DevAddr;
    char termChar_;
    int sweepTime_;

protected:
    ICTransport* ptrTr;
    ptr_createTr pCreateTr;
    ptr_deleteTr pDeleteTr;
    QString moduleLabel;
    int timeout_;



};

extern "C" LIB_DEST void RegTrFuncs(ptr_createTr,ptr_deleteTr);
#endif // TRANSPFUNCS_H
