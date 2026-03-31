#include "cftpclient.h"
#include <QEventLoop>
#include <QFileInfo>
#include <QMessageBox>
#include <QtCore>


CFtpClient::CFtpClient(QString ftpHost,quint16 ftpPort, QString ftpUser, QString ftpPassword)
{
    host = ftpHost;
    port = ftpPort;
    user = ftpUser;
    password = ftpPassword;
    idPut = -1;
    idList = -1;
    idRemove = -1;
    idRemoveDir = -1;
    idMakeDir = -1;
    idGetFile = -1;
    ftp = 0;
    isError = false;
    isPutComplete = false;
    fileObject = 0;
    arr = 0;
    buffer = 0;
    strFromFile = "";
    errorStr = "";
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    timeOutTime = 20000;
}

bool CFtpClient::writeFileToFtp(QString fileOrString, QString dirOnFtp)
{    
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        ftp = new QFtp;
        QEventLoop eventLoop;

        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        QFileInfo info(fileOrString);
        if(info.isFile()) // если переданный параметр - путь к файлу
        {
            QString shortFileName = info.fileName();
            fileObject = new QFile(fileOrString);
            if(! fileObject->isOpen())
            {
                if(fileObject->open(QIODevice::ReadOnly))
                    dir = dirOnFtp + shortFileName;
                else
                {
                    errorStr = tr("Cannot open the file");//Не возможно открыть указанный файл.";
                    isError = true;
                    return isError;
                }
            }
            ftp->put(fileObject, dir);
            idPut = ftp->list(dirOnFtp);

        }
        else  // если переданный параметр - строка для записи в файл
        {
            QString ftpDir = dirOnFtp;
            ftpDir.chop(ftpDir.size() - ftpDir.lastIndexOf("/")-1);
            file = fileOrString;
            dir = dirOnFtp;
            arr = new QByteArray;
            arr->append(fileOrString);
            idPut = ftp->put(*arr, dir);
            idPut = ftp->list(ftpDir);
        }
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    if(isPutComplete == false){
        isError = true;
    }
    return isError;
}

bool CFtpClient::clearFtp(){
    isError = false;
    QString currentDir = "";
    for(int i = 0; i < 3; i++){
        dirList.clear();
        fileList.clear();
        if(i == 0){
            currentDir = "BOOT/";
        }else if( i == 1){
            currentDir = "EPROM/";
        }else if( i == 2){
            currentDir = "FRAM/";
        }/*else if( i == 3){
            currentDir = "RAM/";
        }*/
        if(ftp == 0)
        {
            timer->start(timeOutTime);
            ftp = new QFtp;
            QEventLoop eventLoop;

            isError = false;
            connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
            connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
            connect(ftp,SIGNAL(listInfo(QUrlInfo)),this,SLOT(slotGetContentList(QUrlInfo)));
            ftp->connectToHost(host);
            ftp->login(user, password);
            idList = ftp->list(currentDir);
            eventLoop.exec();
            delete ftp;
            ftp = 0;
        }
        for(int j = 0; j < dirList.size(); j++){
            deleteDirFromFtp(currentDir + dirList.at(j) + "/");
        }
        for(int k = 0; k < fileList.size(); k++){
            deleteFileFromFtp(currentDir + fileList.at(k));
        }
    }
    return isError;
}

void CFtpClient::slotGetContentList(QUrlInfo url){
    if(url.isDir()){
        dirList.append(url.name());
        return;
    }
    if(url.isFile()){
        fileList.append(url.name());
        return;
    }
}

bool CFtpClient::writeDirToFtp(QString dirOnDisk, QString dirOnFtp){

    isError = false;
    QDir dirUser(dirOnDisk);
    if(dirUser.isReadable())
    {
        QFileInfoList dirContent = dirUser.entryInfoList(QStringList(),QDir::Files);
        int filesCount = dirContent.count();
        for(int i = 0; i < filesCount; i++){
            if(writeFileToFtp(dirContent.at(i).absoluteFilePath(), dirOnFtp)){
                errorStr = QString(tr("Cannot write file"));
                isError = true;
                return isError;
            }
        }
    }
    else
    {
        errorStr = QString(tr("Cannot open directory"));
        isError = true;
        return isError;
    }

    return isError;
}

bool CFtpClient::deleteFileFromFtp(QString fileName)
{
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        ftp = new QFtp;
        QEventLoop eventLoop;

        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        idRemove = ftp->remove(fileName);
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    return isError;
}

bool CFtpClient::deleteDirFromFtp(QString dir)
{
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        ftp = new QFtp;
        QEventLoop eventLoop;

        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        idRemoveDir = ftp->rmdir(dir);
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    return isError;
}

bool CFtpClient::makeDir(QString dir)
{
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        QEventLoop eventLoop;
        ftp = new QFtp;
        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        idMakeDir = ftp->mkdir(dir);
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    return isError;
}

bool CFtpClient::getFile(QString fileOnDisk, QString fileOnFtp)
{
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        QEventLoop eventLoop;
        ftp = new QFtp;
        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        fileObject = new QFile(fileOnDisk);
        if(! fileObject->isOpen())
        {
            fileObject->open(QIODevice::WriteOnly);
            idGetFile = ftp->get(fileOnFtp, fileObject);
        }
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    return isError;
}

bool CFtpClient::getTextFromFile(QString fileOnFtp, QString *str)
{
    if(ftp == 0)
    {
        timer->start(timeOutTime);
        QEventLoop eventLoop;
        ftp = new QFtp;
        isError = false;
        connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(slotCheckCommands(int,bool)));
        connect(this, SIGNAL(signalTransferComplete()), &eventLoop, SLOT(quit()));
        ftp->connectToHost(host);
        ftp->login(user, password);
        buffer = new QBuffer;
        buffer->open(QIODevice::ReadWrite);
        idGetTextFromFile = ftp->get(fileOnFtp, buffer);
        eventLoop.exec();
        delete ftp;
        ftp = 0;
    }
    *str = strFromFile;
    strFromFile = "";
    return isError;
}

void CFtpClient::slotCheckCommands(int idCommand, bool error)
{
    if(error)
    {
        timer->stop();
        isError = true;
        errorStr = ftp->errorString() + "\n";
        emit signalTransferComplete();
        ftp->abort();
        return;
    }
    if(idCommand == idPut)
    {
        if(fileObject != 0)
        {
            fileObject->close();
            delete fileObject;
            fileObject = 0;
        }
        if(arr != 0)
        {
            delete arr;
            arr = 0;
        }
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        isPutComplete = true;
        return;
    }

    if(idCommand == idList){
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }

    if(idCommand == idRemove)
    {
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }
    if(idCommand == idRemoveDir)
    {
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }
    if(idCommand == idMakeDir)
    {
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }
    if(idCommand == idGetFile)
    {
        if(fileObject != 0)
        {
            fileObject->close();
            delete fileObject;
            fileObject = 0;
        }
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }
    if(idCommand == idGetTextFromFile)
    {
        if(buffer != 0)
        {
            strFromFile.append(buffer->data());
            buffer->close();
            delete buffer;
            buffer = 0;
        }
        emit signalTransferComplete();
        timer->stop();
        ftp->abort();
        return;
    }
}

QString CFtpClient::errorString()
{
    return this->errorStr;
}

void CFtpClient::slotTimeOut(){
    timer->stop();
    isError = true;
    errorStr = QString(tr("Answering interval from the instrument expired.\nThe host of the instrument is probably incorrectly specified\nor the instrument is switched off\n"));
    //"Время ожидания ответа от прибора истекло.\nВозможно неправильно указан хост прибора\nили прибор выключен.\n");
    emit signalTransferComplete();
    ftp->abort();
}
