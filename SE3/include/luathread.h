#ifndef LUATHREAD_H
#define LUATHREAD_H

#include<QThread>
//#include <ActiveQt/QAxObject>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
//#include <shlobj.h>
#include <QColor>
#include "../include/luacbridge.h"

enum Operations{
    opNone,
    opLoad,
    opStart,
    opStop,
    opRun
};

enum States{
    noError,
    error,
};

struct errStruct
{
    bool err;
    QString errDescr;
    QString errTraceBack;
};

class LuaWrap : public QObject
{
    Q_OBJECT

signals:
    void SendErrStrc(errStruct);
    void operationStatus(Operations operId, States operState);
    void OperationComplite();
    void OperationStarted();
    void Finished();

public slots:
    void GetErrDescr();
    void Run();
    void StopScript();

public:
    LuaWrap(QObject *parent = 0);
    ~LuaWrap();

protected slots:
    void StartScript();
    void LoadScript(QString scriptPath);

private:
    QString currScr;
    Operations currOp;
    errStruct lastErr;
    bool finished;
    void WaitForStop();
};
#endif // LUATHREAD_H
