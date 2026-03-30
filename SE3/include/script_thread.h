#ifndef PYWRAP_H
#define PYWRAP_H


#ifdef WIN32
    #include <ActiveQt/QAxObject>
    #include <shlobj.h>
#endif

#include<QThread>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

#include <QColor>
#include <QMutex>
#include <QLibrary>
#include "../include/findmodules.h"
#include <../include/scriptcwrapper.h>
#include "../include/scriptcbridge.h"
#include "../include/errormanager.h"

typedef IScriptControl* (*tCreateScriptControl) (ScriptEngine engine);
typedef void (*tRemoveScriptControl)(IScriptControl* sControl);
typedef QObject* (*tGetSesManagerObj)();

enum Operations{
    opNone,
    opLoad,
    opStart,
    opStop,
    opRun,
    opInit,
    opStopExit,
};

enum States{
    noError,
    error,
};

struct errStruct
{
    Operations opId;
    QString errDescr;
    QString errTraceBack;
};

class ScriptWrap : public QObject, public ErrorManager
{
    Q_OBJECT

signals:
    void SendErrStrc(errStruct);
    void operationStatus(Operations operId, States operState);
    void OperationComplite();
    void OperationStarted();
    void Finished();
    void exitReady();

public slots:
    void StopScript();
    void StopExitScript();
    void requestLastErrStruct(){SendErrStrc(lastES);}

public:
    ScriptWrap(QObject *parent = 0);
    ~ScriptWrap();
    QObject* GetSesManagerObj();

protected slots:
    void init(ScriptEngine scriptType);
    void StartScript();
    void LoadScript(QString path);

private:
    IScriptControl* m_pScriptControl;
    QString currScr;
    Operations currOp;
    States currState;
    bool finished;
    bool WaitForStop(int msec = 1000);
    void CheckResult(int code);
    void setFinished(bool val);
    bool getFinished();
    errStruct lastES;
    QString lastErrDescr;
    QMutex m_mutex;

    tCreateScriptControl pCreateScriptControl;
    tRemoveScriptControl pRemoveScriptControl;
    tGetSesManagerObj pGetSesManagerObj;
    QLibrary *library;
};
#endif // PYWRAP_H
