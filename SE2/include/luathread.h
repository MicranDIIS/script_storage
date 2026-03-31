#ifndef LUATHREAD_H
#define LUATHREAD_H

#include<QThread>
#include <ActiveQt/QAxObject>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <shlobj.h>
#include <QColor>
#include "LuaCBridge.h"

class LuaWrap : public QObject
{
    Q_OBJECT

signals:
    void ShowError(QString Msg,QString tb);
    void OperationComplite();
    void OperationStarted();
    void Finished();

public slots:
    void Stop();
    void StopScript();
    void InitCOM(){ CoInitializeEx(NULL, COINIT_MULTITHREADED);}

public:
    LuaWrap(QObject *parent = 0);
    ~LuaWrap();
    QString currScr;
    bool finished;

protected slots:
    void StartScript();
    void LoadScript(QString scriptPath);

private:
    void CheckErr(bool RunScr = false);
//    QString currScr;
};
#endif // LUATHREAD_H
