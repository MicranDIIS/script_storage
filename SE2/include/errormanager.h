#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H
#include<QString>
#include <qdebug.h>

using namespace std;

class ErrorManager : public QObject
{
public:
    ErrorManager();
    ~ErrorManager();
    string GetError();
    void SetError(QString);
    void *GetPtr(){return this;}

private:
    QString LastErr;
};

#endif // ERRORMANAGER_H
