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
    return err.toAscii().data();
}

void ErrorManager::SetError(QString errDescr)
{
    LastErr = errDescr;
}
