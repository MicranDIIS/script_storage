#ifndef TRANSPFUNCSSCPI_H
#define TRANSPFUNCSSCPI_H

#include "transpfuncsbase.h"

class CTranspFuncsScpi : public CTranspFuncsBase{
    Q_OBJECT
public:
    CTranspFuncsScpi();

    int Send(string sComm);
    int Recv();
    QString CorrectAddr(QString sAddr);
    void CheckSpecificErr(int nErrCode);
    int GetTransportType();
    int InitMiTransport(bool bState);
    int SendScpiWithBinData(string command, string filePath);
};

#endif // TRANSPFUNCSSCPI_H
