#include "errormanager_uni.h"

ErrorManager::ErrorManager()
{
    LastErr = "No error";
}

ErrorManager::~ErrorManager()
{

}

string ErrorManager::GetError()
{
    QString err = LastErr;
//    LastErr = "No error";
    return err.toUtf8().data();
}

void ErrorManager::SetError(QString errDescr)
{
    LastErr = errDescr;
}
