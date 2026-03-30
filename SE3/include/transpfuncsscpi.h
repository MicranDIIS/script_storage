#ifndef TRANSPFUNCSSCPI_H
#define TRANSPFUNCSSCPI_H

#include "transpfuncsbase.h"

class CTranspFuncsScpi : public CTranspFuncsBase{
    Q_OBJECT
public:
    CTranspFuncsScpi();

    Q_INVOKABLE int Send(char* sComm);
    int Recv();
    QString CorrectAddr(QString sAddr);
    void CheckSpecificErr(int nErrCode);
    int GetTransportType();
    Q_INVOKABLE int InitMiTransport(bool bState);
};

#endif // TRANSPFUNCSSCPI_H
