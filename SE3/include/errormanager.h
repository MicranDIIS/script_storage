#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H
#include<QString>
#include <qdebug.h>

#define DBG_PROC

#ifdef DBG_PROC
#  define DBGSHOW(message) qDebug()<<message;
#else
#  define DBGSHOW
#endif

using namespace std;
#define nullptr 0

typedef struct sError{
    int code;
    QString description;
}sErrorAlias;

#undef ENUM_ELEMENT
#define ENUM_ELEMENT(A,B,C) A = B
enum eError{
    #include "errors.h"
};

#undef ENUM_ELEMENT
#define ENUM_ELEMENT(A,B,C) {B,C}
static sErrorAlias ErrorInfo[] = {
    #include "errors.h"
};

#define EARRAYSIZE ((int)sizeof(ErrorInfo) / (int)sizeof(*ErrorInfo))

class ErrorManager
{
#ifdef TEST_MODE
    friend class errManager_test;
#endif
public:
    ErrorManager(bool autoClear = true);
    ~ErrorManager();
    int SetError(QString errDescr);
    int SetError(const char* errDescr);
    int SetError(int errCode);
    QString GetErrorDescription(int code, bool *ok = nullptr);

    int GetLastErrorCode();
    Q_INVOKABLE const char* GetError(); // для Луа
    QString GetQSError();
    void *GetPtr(){return this;}

private:
    QString m_qsLastErr;
    int m_sErrCode;
    int getErrorIndexInMass(int code);
    void clearError();
    bool m_autoClear;
};

#endif // ERRORMANAGER_H
