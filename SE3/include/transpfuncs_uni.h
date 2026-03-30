#ifndef TRANSPFUNCS_H
#define TRANSPFUNCS_H

#include <../include/Transport.h>
#include <QString>
#include "errormanager_uni.h"
#include <QtCore>
#include "Libs_global.h"
#include "LuaRef.h"

typedef ICTransport* (*ptr_createTr)(QString,bool&);
typedef void (*ptr_deleteTr)(ICTransport*);

static ptr_createTr spCrTr;
static ptr_deleteTr spDelTr;
static lua_State* ls;

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

class TranspFuncs : public QObject, public ErrorManager{
    Q_OBJECT
public:
    TranspFuncs();
    ~TranspFuncs();
    int Connect(string addr,int LibInit = 0, string termCh = "\n");
    int Send(string comm,bool ReadAnsw = true, bool byByte = false);
    int SendBuf(char* buf, int cnt);
    int Recv(bool transl = false, int nbyte = 0);
    int Disconnect();
    string GetAnsw();
    char* GetRawData();
    int CheckErr(int errCode);
    void SetAnsw(string val);
    int SleepMS(int mstime);

private:

    ptr_createTr pCreateTr;
    ptr_deleteTr pDeleteTr;
    bool isAddrDbl;    
    bool isSCPI;
    string Answ;
    bool isMICS;
    char RawData[65535];
    QString DevAddr;    
    char termChar_;
protected:
    ICTransport* ptrTr;

};

extern "C" LIB_DEST void RegTrFuncs(ptr_createTr,ptr_deleteTr);
#endif // TRANSPFUNCS_H
