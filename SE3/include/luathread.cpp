#include "luathread.h"
#include <QApplication>
#include <QTextCodec>

LuaWrap::LuaWrap(QObject *parent):QObject(parent)
{
    LuaOpen();
    currOp = opNone;
}

LuaWrap::~LuaWrap()
{
    LuaClose();
    CoUninitialize();
}

void LuaWrap::GetErrDescr()
{
    qDebug()<<"slot getErrorDescr";
    LuaGetError(&lastErr.errDescr,&lastErr.errTraceBack);
    emit SendErrStrc(lastErr);
    qDebug()<<lastErr.errDescr;
    lastErr.err = noError;
}

void LuaWrap::Run()
{
    finished = false;

}

void LuaWrap::LoadScript(QString path)
{
    //Пробуем запустить скрипт на выполнение
    //Анализируем что нам вернул скрипт
    currScr = path;
    QString app_path = QApplication::applicationDirPath()+"/..";
    app_path.replace(QString("/"),QString("\\\\"));

    LuaDoString((char*)QString("package.cpath = package.path..\";"+app_path+"\\\\libs\\\\?.dll\"").toLocal8Bit().data());
    LuaDoString((char*)QString("package.path = package.path..\";"+app_path+"\\\\scripts\\\\modules\\\\?.lua\"").toLocal8Bit().data());

    QByteArray temp = path.toLocal8Bit();
    QTextDecoder *decoder = QTextCodec::codecForName("utf-8")->makeDecoder();
    QTextEncoder *encoder = QTextCodec::codecForName("windows-1251")->makeEncoder();
    path = decoder->toUnicode(temp);//QTextCodec::codecForName("windows-1251")->fromUnicode(QTextCodec::codecForName("utf-8")->toUnicode(temp));
    QByteArray path2ba = encoder->fromUnicode(path);

    States state = LuaLoadFile(path2ba.data()) ? error : noError;
    emit operationStatus(opLoad, state);
}

void LuaWrap::WaitForStop()
{
    qDebug()<<"LuaWrap"<<"wait for stop";
    QEventLoop el;
    connect(this,SIGNAL(Finished()),&el,SLOT(quit()));
    if(!finished)
        el.exec();
}

void LuaWrap::StartScript()
{
    LuaDoString((char*)"abort = 0"); // обнуляем флаг прерывания скрипта
    LuaDoString((char*)"cc_exec = 0");
    finished = false;
    QString appPath = QApplication::applicationDirPath() + "/../";
    QString command = QString("AppPath = \"") + appPath + "\"";
    LuaDoString((char*)command.toLocal8Bit().data());
    emit operationStatus(opStart, noError);
    CoInitializeEx(NULL,COINIT_MULTITHREADED);
    States err = LuaDoFuncNoArg((char*)"Run") ? error : noError;
    CoUninitialize();
    emit operationStatus(opRun, err);
    finished = true;
    emit Finished();
}

void LuaWrap::StopScript()
{
    if(!finished)
    {
        //Инициализируем прерывание выполнения скрипта
        LuaDoString((char*)"abort = 1");
        WaitForStop();
        if(!LuaGetGlobal((char*)"cc_exec"))
            LuaDoString((char*)"base.ClearConnections()");
    }
    CoUninitialize();
    emit operationStatus(opStop, noError);
}
