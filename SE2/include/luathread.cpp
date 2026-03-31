#include "luathread.h"
#include <QApplication>

LuaWrap::LuaWrap(QObject *parent):QObject(parent)
{
    //Перемещаем объект луа-потока в луа-поток
//    moveToThread(this);
//    CoInitialize(NULL);

}

LuaWrap::~LuaWrap()
{
//    LuaClose();
    CoUninitialize();
}


void LuaWrap::LoadScript(QString path)
{
//    qDebug() << "<LoadScript()>";
//    qDebug()<<path;
    //Пробуем запустить скрипт на выполнение
    //Анализируем что нам вернул скрипт
    currScr = path;
    QString app_path = QApplication::applicationDirPath();
    app_path.replace(QString("/"),QString("\\\\"));
    qDebug()<<app_path;
    LuaDoString((char*)QString("package.cpath = package.path..\";"+app_path+"\\\\?.dll\"").toLocal8Bit().data());
    LuaDoString((char*)QString("package.path = package.path..\";"+app_path+"\\\\scripts\\\\modules\\\\?.lua\"").toLocal8Bit().data());

    QString command = QString("AppPath = \"") + app_path + "\\\\\"";
    LuaDoString((char*)command.toLocal8Bit().data());

    if(LuaLoadFile((char*)QString(path).toLocal8Bit().data()))
    {
        CheckErr();
    }


//    qDebug() << "</LoadScript()>";
}

void LuaWrap::Stop()
{
//    qDebug() << "<Stop()>";
    LuaClose(); //Выгружаем Луа
//    quit(); //Останавливаем поток
//    return;
    finished = true;
    emit Finished();
    qDebug() << "</Stop()>";
}

void LuaWrap::StartScript()
{
//    qDebug() << "<StartScript()>";
    LuaDoString((char*)"abort = 0"); // обнуляем флаг прерывания скрипта
    LuaDoString((char*)"cc_exec = 0");
    emit OperationStarted();
    LuaDoFuncNoArg((char*)"Run");
//    {
    CheckErr(true);
//    }

    emit OperationComplite();
//    qDebug() << "</StartScript()>";
}

void LuaWrap::StopScript()
{
    qDebug() << "<StopScript()>";
    //Инициализируем прерывание выполнения скрипта
    LuaDoString((char*)"abort = 1");
//    emit OperationComplite();
    qDebug() << "</StopScript()>";
}

void LuaWrap::CheckErr(bool RunScr)
{
//    qDebug() << "<LuaWrap::CheckErr()>";
    QString err;
    QString tb;
    LuaGetError(&err,&tb); //Просим Луа поместить в err описание последней ошибки
    if(!err.isEmpty()){
        //        qDebug()<<"tb:"<<tb;
        //        int firstSymb = err.indexOf(":",2);
        //        if(firstSymb>0){
        ////                int secondSymb = err.indexOf(":",firstSymb+1);
        //            if(err.left(firstSymb)!=currScr){
        //                QRegExp rn(":(\\d+):");
        //                QStringList stack = tb.split("\n");
        //                rn.indexIn(stack.first());
        //                err.replace(rn,rn.cap(0));
        //                err.replace(err.left(firstSymb),currScr);
        //            }

        //        }

        //        QByteArray temp = err.toLocal8Bit();
        emit ShowError(err,tb);
        //        msleep(100);
    }
    if(RunScr){
        int ab = LuaGetGlobal((char*)"cc_exec");
        qDebug()<<"CC_EXEC=1"<<ab;
        if(!ab) LuaDoString((char*)"base.ClearConnections()");
    }
//    qDebug() << "</LuaWrap::CheckErr()>";
}
