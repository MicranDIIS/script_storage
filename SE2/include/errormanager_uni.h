#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H
#include<QString>

using namespace std;

class ErrorManager
{
public:
    ErrorManager();
    ~ErrorManager();
    string GetError();
    void SetError(QString);

private:
    QString LastErr;
};

#endif // ERRORMANAGER_H
