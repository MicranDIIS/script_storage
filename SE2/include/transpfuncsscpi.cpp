#include "transpfuncsscpi.h"

CTranspFuncsScpi::CTranspFuncsScpi()
{
    SetTransportType(AGILENT_LIB);
}

int CTranspFuncsScpi::Send(string sComm)
{
        QString sCommand = QString::fromStdString(sComm);
        if(!sCommand.contains("\r\n"))
            sCommand+="\r\n";
        int nCnt = sCommand.count();

        qDebug()<<"send"<<sCommand;
        if (SendBuf(sCommand.toLocal8Bit().data(),nCnt))
            return -1;

        if(sCommand.contains("?"))
        {
            if(Recv())
                return -1;
        }
    return 0;
}

int CTranspFuncsScpi::Recv()
{
    int nSizeInBuff = 65535;
    int nCnt = 65535;

    char *szBuf = new char[nSizeInBuff];
    qDebug()<<"start recv";

    if(RecvBuf(szBuf,&nCnt)) {
        return -1;

	qDebug()<<"recv "<<nCnt<< " bytes";}

    szBuf[nCnt] = '\0';
    SetAnsw(std::string(szBuf,nCnt));
    SetRawData(0,szBuf,nCnt);
    delete [] szBuf;
    return 0;
}

QString CTranspFuncsScpi::CorrectAddr(QString sAddr)
{
    return sAddr;
}

void CTranspFuncsScpi::CheckSpecificErr(int nErrCode)
{
    SetError(tr("Unknown error ")+QString::number(nErrCode));
}

int CTranspFuncsScpi::GetTransportType()
{
    return AGILENT_LIB;
}

int CTranspFuncsScpi::InitMiTransport(bool bState)
{
    if(bState)
        SetTransportType(MIVISA_LIB);
    else
        SetTransportType(AGILENT_LIB);
    return 0;
}

int CTranspFuncsScpi::SendScpiWithBinData(string command, string filePath)
{
    QFile file(QString::fromStdString(filePath));
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileContent = file.readAll();
        file.close();
        QString commandPart = QString::fromStdString(command);

        fileContent.append("\n");
        int readingSize = fileContent.size();
        int readingSizeForHeader = readingSize - 1;

//        qDebug() << "filecontent" << fileContent << readingSize;

        QString commandPrefix = QString("%1,#").arg(commandPart);
        QString sizeStr = QString::number(readingSizeForHeader);
        QString sizeNumberCountStr = QString::number(sizeStr.count());
        commandPrefix += sizeNumberCountStr + sizeStr;

        int prefixSize = commandPrefix.toAscii().size();
        int resultCommandSize = prefixSize + readingSize;
        char * resultCommandBuffer = new char[resultCommandSize];
        memcpy(resultCommandBuffer, commandPrefix.toAscii().data(), prefixSize);
        memcpy(&resultCommandBuffer[prefixSize], fileContent.data(), readingSize);

        if (m_pTransport) {
            if (CheckErr(m_pTransport->SendData(resultCommandBuffer,&resultCommandSize))) {
                qDebug() << "--- Fail_1!";
                return -1;
            } else {
                qDebug() << resultCommandBuffer << "+++ Success!";
            }
        } else {
            qDebug() << "Transport not exist";
        }


        delete [] resultCommandBuffer;
    } else {
        qDebug() << "--- Fail_2!";
        SetError(file.errorString());
        return -1;
    }
    return 0;
}
