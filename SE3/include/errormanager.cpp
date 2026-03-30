#include "errormanager.h"
#include <QCoreApplication>

QString ErrorManager::GetErrorDescription(int code, bool *ok) {
    QString szNone = QCoreApplication::translate("Errors","Unknown error");
    if(ok) *ok = false;
    if(code == USER_ERROR){
        if(m_sErrCode!=SE3_NO_ERROR)
        {
            if(ok) *ok = true;
            szNone = m_qsLastErr;
        }
    }
    else{
        for (int i = 0; i < EARRAYSIZE; i++)
        {
            if (ErrorInfo[i].code == code)
            {
                szNone =  QCoreApplication::translate("Errors",ErrorInfo[i].description.toUtf8().data());
                if(ok) *ok = true;
                break;
            }
        }
    }
    if(m_autoClear)
        clearError();
    return szNone;
}

void ErrorManager::clearError()
{
    SetError(SE3_NO_ERROR);
}

ErrorManager::ErrorManager(bool autoClear): m_autoClear(autoClear)
{
//   SetError(SE3_NO_ERROR);
    m_qsLastErr = "";
    m_sErrCode = SE3_NO_ERROR;
}

ErrorManager::~ErrorManager()
{
}

int ErrorManager::SetError(QString errDescr)
{
    return SetError(errDescr.toUtf8().constData());
}

const char* ErrorManager::GetError()
{
    qDebug()<<"ErrorManager. GetError()."<<m_sErrCode;
    return GetErrorDescription(m_sErrCode).toUtf8().constData();
}

int ErrorManager::GetLastErrorCode()
{
    return m_sErrCode;
}

QString ErrorManager::GetQSError()
{
    return GetErrorDescription(m_sErrCode);
}

//int ErrorManager::arraySize()
//{
//    return ((int)sizeof(ErrorInfo) / (int)sizeof(*ErrorInfo)); // Определение размерности массива
//}

int ErrorManager::SetError(const char* errDescr)
{
    m_qsLastErr = errDescr;
    m_sErrCode = USER_ERROR;
    DBGSHOW(QString("ErrorManager. SetError(). Error: %1").arg(errDescr));
    return m_sErrCode;
}

int ErrorManager::SetError(int errCode)
{
    m_qsLastErr = "";
    for (int i = 0; i < EARRAYSIZE; i++){
        if (ErrorInfo[i].code == errCode){
            qDebug()<<ErrorInfo[i].description;
            QByteArray ba = ErrorInfo[i].description.toLocal8Bit();
            const char *c_str2 = ba.data();
            m_qsLastErr = QCoreApplication::translate("Errors",c_str2);
            break;
        }
    }
    m_sErrCode = errCode;
    return m_sErrCode;
}
