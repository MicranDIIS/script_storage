#include "errormanager.h"

ErrorManager::ErrorManager()
{
//    qDebug() << "<ErrorManager()>";
    LastErr = "No error";
//    qDebug() << "</ErrorManager()>";
}

ErrorManager::~ErrorManager()
{

}

string ErrorManager::GetError()
{
//    qDebug() << "<GetError()>";
    QString err = LastErr;
    LastErr = "No error";
//    qDebug()<<"Get error: "<<this;
//    qDebug()<<"Get error parent: "<<this->parent;
//    LastErr = "No error";
    QByteArray ba;
    ba.append(err);
//    qDebug() << "</GetError()>";
    return ba.data();
}

void ErrorManager::SetError(QString errDescr)
{
//    qDebug() << "<SetError()>";
    LastErr = errDescr;
    qDebug() << QString("ErrorManager. SetError(). Error: %1").arg(errDescr);
//    qDebug() << "</SetError()>";
}
