#include "backuper.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

//for time stamps operation
#include <qfile.h>
#include <qtextstream.h>
#include <iostream>
#include <QDir>
#include <QString>
#include <sys/stat.h>

BackUper::BackUper(QString appDir, QString archivePath):mAppDirPath(appDir),mArchivePath(archivePath)
{
    m_inited = false;
//    if(mAppDirPath.at(0)=="/")
//        mAppDirPath = mAppDirPath.right(mAppDirPath.length()-1);

    UpdateListOfFiles();
//    struct statx stx;
//    QFileInfo fi("/home/regor/QtProject/se3_solution/se3test/se3/bin/release/scripts/ansi.lua");
//    statx(AT_FDCWD,QString("/home/regor/QtProject/se3_solution/se3test/se3/bin/release/scripts/ansi.lua").toLocal8Bit().data(),AT_SYMLINK_NOFOLLOW,STATX_BASIC_STATS | STATX_BTIME,&stx);
//    qDebug()<<"creation date:"<<ctime((time_t*)&stx.stx_btime.tv_sec)<<(stx.stx_ino)<<fi.birthTime();
//    qDebug()<<"modif date:"<<ctime((time_t*)&stx.stx_mtime.tv_sec);
//    qDebug()<<"access date:"<<ctime((time_t*)&stx.stx_atime.tv_sec);
//    qDebug()<<"change date:"<<ctime((time_t*)&stx.stx_ctime.tv_sec);
}

short BackUper::InitArchive(QString archivePath)
{
    mArchiveFiles.clear();
    QFileInfo fileInfo(archivePath);
    if(!fileInfo.exists()){
        return SetError("Backup archive not exist");
    }

    QFile archive_file(archivePath);
    if(!archive_file.open(QIODevice::ReadOnly)){
        return SetError("Can't open backup file");
    }

    QByteArray data2 = qUncompress(archive_file.readAll());
    char* pHead = data2.data();
    int cnt = 0;

    while(cnt<data2.size())
    {
        InfoHeader* header = reinterpret_cast<InfoHeader*>(&pHead[cnt]);
        char* pData = &pHead[cnt+sizeof(InfoHeader)];
        cnt+=sizeof(InfoHeader);

        QString filePath = QString(QByteArray(pData,header->pathSize));
        cnt+=header->pathSize;
        pData+=header->pathSize;
        cnt+=header->fileSize;
        pData+=header->fileSize;

        if(filePath.at(0)==QDir::separator())
            filePath = filePath.right(filePath.length()-1);
        QString restoredPath = QDir::cleanPath(mAppDirPath+filePath);
        mArchiveFiles.append(restoredPath);
    }
    archive_file.close();
    return SE3_NO_ERROR;
}

short BackUper::AddFile(QString relFilePath)
{
    if(relFilePath.isEmpty())
        return SetError(BCK_ADDPATH_EMPTY);
    if(m_files.contains(relFilePath))
        return SetError(BCK_ADDPATH_DBL);
    short res = PrepareChunkOfData(m_archive,relFilePath);
    if(res)
        return res;
    m_files.append(relFilePath);
    return SE3_NO_ERROR;
}

QStringList BackUper::GetAddedFileList()
{
    return m_files;
}

QStringList BackUper::GetRestoredFileList()
{
    return m_restored_files;
}

void BackUper::ClearFiles()
{
    m_archive.clear();
    m_files.clear();
}

short BackUper::BackUp(QString path)
{
    qDebug()<<"backup func";
    QString absFilePath, relFilePath;

    /// Convert path to absolute and relative
    if(QDir::isRelativePath(path))
    {
        absFilePath = mAppDirPath + path;
        relFilePath = path;
    }
    else
    {
        absFilePath = path;
        QDir appDir(mAppDirPath);
        relFilePath = appDir.relativeFilePath(absFilePath);
    }
    qDebug()<<absFilePath<<relFilePath<<mAppDirPath;

    QByteArray newData;
    if(PrepareChunkOfData(newData,relFilePath) != SE3_NO_ERROR)
        return GetLastErrorCode();

    QByteArray data;
    QFile archiveFile(mArchivePath);
    if(mArchiveFiles.length())
    {
        if(!archiveFile.open(QIODevice::ReadOnly)){
            return SetError("Can't open backup file for read");
        }
        data = qUncompress(archiveFile.readAll());
        archiveFile.close();

        if(mArchiveFiles.contains(absFilePath))
        {
            int len = 0;
            int offset = 0;
            if(CalcOffsetInArray(data, relFilePath, offset, len))
                data.replace(offset, len, newData);
            else
                return SetError(tr("Unable locate old chank in archive"));
        }
        else
        {
            data.append(newData);
            mArchiveFiles.append(absFilePath);
        }
    }
    else
    {
        data.append(newData);
        mArchiveFiles.append(absFilePath);
    }



    if(!archiveFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        return SetError("Can't open backup file for write");
    }

    QByteArray compressedData = qCompress(data,6);
    archiveFile.write(compressedData);
    archiveFile.close();
    return SE3_NO_ERROR;
}

short BackUper::CreateBackUp(QString archivePath)
{
    if(m_archive.isEmpty()){
        return SetError("No data for backup");
    }
    QFile archive_file(archivePath);
    if(archive_file.exists() && !archive_file.remove())
        SetError("Can't remove previous backup file");

    if(!archive_file.open(QIODevice::WriteOnly)){
        return SetError("Can't create backup file");
    }

    archive_file.write(qCompress(m_archive,6));
    archive_file.close();
    ClearFiles();
    return SE3_NO_ERROR;
}

short BackUper::RestoreAllFromBackUp(QString archivePath)
{
    QFile archive_file(archivePath);
    if(!archive_file.open(QIODevice::ReadOnly)){
        return SetError("Can't open backup file");
    }

    QByteArray data = qUncompress(archive_file.readAll());
    char* pHead = data.data();
    int cnt = 0;

    m_restored_files.clear();

    while(cnt<data.size())
    {
        InfoHeader* header = reinterpret_cast<InfoHeader*>(&pHead[cnt]);
        char* pData = &pHead[cnt+sizeof(InfoHeader)];
        cnt+=sizeof(InfoHeader);

        QString filePath = QString(QByteArray(pData,header->pathSize));
        cnt+=header->pathSize;
        pData+=header->pathSize;

        QString restoredPath = (mAppDirPath+"/"+filePath);
        QFile restoredFile(restoredPath);
        if(restoredFile.exists() && !restoredFile.remove()){
            return SetError(tr("Can't remove file: %1").arg(filePath));
        }
        if(!restoredFile.open(QIODevice::WriteOnly)){
            return SetError(tr("Can't create restored file: %1").arg(filePath));
        }
        restoredFile.write(QByteArray(pData,header->fileSize));
        cnt+=header->fileSize;
        restoredFile.close();

        // Восстанавливаем временные метки
        wchar_t *dstPath = new wchar_t[restoredPath.length()];
        for (int x(0);x<restoredPath.length();x++) {
            dstPath[x] = restoredPath.toStdString().c_str()[x];
        }
//        HANDLE dfile =  CreateFile(dstPath,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//        SetFileTime(dfile,&header->ftCreate, &header->ftAccess, &header->ftWrite);
//        CloseHandle(dfile);
        delete [] dstPath;

        m_restored_files.append(filePath);
    }
    return SE3_NO_ERROR;
}

short BackUper::AppendFileToArchive(QString archivePath, QString relFilePath)
{
    QFile archive_file(archivePath);
    if(!archive_file.open(QIODevice::ReadWrite)){
        return SetError("Can't open backup file");
    }

    QByteArray data = qUncompress(archive_file.readAll());
    archive_file.close();
    archive_file.open(QIODevice::WriteOnly);

    QByteArray newData;
    PrepareChunkOfData(newData,relFilePath);

    int len = 0;
    int from = 0;
    int from2 = from;
    if(CalcOffsetInArray(data,relFilePath,from2,len))
        data.replace(from,len,newData);
    else
        data.append(newData);


//    ClearFiles();
//    AddFile(relFilePath);
//    data.append(m_archive);

    QByteArray comp_data = qCompress(data,6);
    archive_file.write(comp_data);
    archive_file.close();
    return SE3_NO_ERROR;
}

short BackUper::RestoreFileFromArchive(QString archivePath, QString relFilePath)
{
    QFile archive_file(archivePath);
    if(!archive_file.open(QIODevice::ReadWrite)){
        return SetError("Can't open backup file");
    }

    QByteArray data = qUncompress(archive_file.readAll());

    archive_file.close();
    archive_file.open(QIODevice::WriteOnly|QIODevice::Truncate);

    int len = 0;
    int from = 0;
    int chr = CalcOffsetInArray(data,relFilePath,from,len);
    if(chr)
    {
        m_restored_files.clear();
        int from2 = from;
        SetError(RestoreFromChunkOfData(data,from2));
    }

    if(data.size()==len)
        archive_file.remove();
    else
    {
        data.remove(from,len);
        archive_file.write(qCompress(data,6));
        archive_file.close();
    }
    return GetLastErrorCode();
}

bool BackUper::CheckFileInArchive(QString fullPath)
{
    return mArchiveFiles.contains(fullPath);
}

bool BackUper::CalcOffsetInArray(const QByteArray &archiveData, QString relFilePath, int& offset, int& len)
{
    const char* pHead = archiveData.data();
    int cnt = 0;
    int _from = 0;
    int _len = 0;
    while(cnt<archiveData.size())
    {
        InfoHeader* header =const_cast<InfoHeader*>(reinterpret_cast<const InfoHeader*>(&pHead[cnt]));
        const char* pData = &pHead[cnt+sizeof(InfoHeader)];
        _from = cnt;
        cnt+=sizeof(InfoHeader);
        QString pathInArch = QString(QByteArray(pData,header->pathSize));
        cnt+=header->pathSize;
        cnt+=header->fileSize;
        _len = cnt - _from;

        if(relFilePath == pathInArch)
        {
            offset = _from;
            len = _len;
            return true;
        }
    }
    return false;
}

short BackUper::PrepareChunkOfData(QByteArray &readyData, QString relFilePath)
{
    QString filePath = mAppDirPath + relFilePath;
    QFileInfo file_info(filePath);
//    qDebug()<<"prepare chunk"<<filePath<<QFileInfo::exists(filePath)<<file_info.birthTime();
    if(!file_info.exists()){
        return SetError(tr("%1 is not exist").arg(file_info.absoluteFilePath()));
    }




    QFile data(filePath);
    if(!data.open(QIODevice::ReadOnly)){
        return SetError(tr("Can't open for added to BackUp"));
    }

    // Сохраняем временные метки
    wchar_t *srcPath = new wchar_t[relFilePath.length()];
    for (int i(0);i<relFilePath.length();i++) {
        srcPath[i] = relFilePath.toStdString().c_str()[i];
    }
//    HANDLE sfile =  CreateFile(srcPath,0,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_READONLY,NULL);

    InfoHeader* pHead = reinterpret_cast<InfoHeader*>(m_header_mas);
    pHead->pathSize = relFilePath.size();
    pHead->fileSize = file_info.size();


//    GetFileTime(sfile,&pHead->ftCreate, &pHead->ftAccess, &pHead->ftWrite);
//    CloseHandle(sfile);
    delete [] srcPath;

    readyData.append(QByteArray(m_header_mas,sizeof(InfoHeader)));
    readyData.append(relFilePath);
    readyData.append(data.readAll());
    return SE3_NO_ERROR;
}

short BackUper::RestoreFromChunkOfData(const QByteArray &inputData, int &offset)
{
    const char* pHead = inputData.data();
    InfoHeader* header = const_cast<InfoHeader*>(reinterpret_cast<const InfoHeader*>(&pHead[offset]));
    const char* pData = &pHead[offset+sizeof(InfoHeader)];
    offset+=sizeof(InfoHeader);

    QString filePath = QString(QByteArray(pData,header->pathSize));
    offset+=header->pathSize;
    pData+=header->pathSize;

    QString restoredPath = (mAppDirPath+filePath);
    QFile restoredFile(restoredPath);
    if(restoredFile.exists() && !restoredFile.remove()){
        return SetError(tr("Can't remove file: %1").arg(filePath));
    }
    if(!restoredFile.open(QIODevice::WriteOnly)){
        return SetError(tr("Can't create restored file: %1").arg(filePath));
    }
    restoredFile.write(QByteArray(pData,header->fileSize));
    offset+=header->fileSize;
    restoredFile.close();

    // Восстанавливаем временные метки
    wchar_t *dstPath = new wchar_t[restoredPath.length()];
    for (int x(0);x<restoredPath.length();x++) {
        dstPath[x] = restoredPath.toStdString().c_str()[x];
    }
//    HANDLE dfile =  CreateFile(dstPath,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//    SetFileTime(dfile,&header->ftCreate, &header->ftAccess, &header->ftWrite);
//    CloseHandle(dfile);
    delete [] dstPath;

    m_restored_files.append(filePath);

    return SE3_NO_ERROR;
}

short BackUper::UpdateListOfFiles()
{
    mArchiveFiles.clear();
    if(!QFile::exists(mArchivePath)){
        return SE3_NO_ERROR;
    }

    QFile archiveFile(mArchivePath);
    if(!archiveFile.open(QIODevice::ReadOnly)){
        return SetError("Can't open backup file");
    }


    QByteArray dataRaw = qUncompress(archiveFile.readAll());
    char* pHead = dataRaw.data();
    int cntRead = 0;
    qDebug()<<"backup file is uncompressed"<<dataRaw.size();
    while(cntRead < dataRaw.size())
    {
        InfoHeader* header = reinterpret_cast<InfoHeader*>(&pHead[cntRead]);
        char* pData = &pHead[cntRead+sizeof(InfoHeader)];
        cntRead+=sizeof(InfoHeader);
        qDebug()<<"cntRead"<<header->pathSize;
        QString filePath = QString(QByteArray(pData,header->pathSize));
        cntRead+=header->pathSize;
        pData+=header->pathSize;
        cntRead+=header->fileSize;
        pData+=header->fileSize;

        if(filePath.at(0)==QDir::separator())
            filePath = filePath.right(filePath.length()-1);
        QString restoredPath = QDir::cleanPath(mAppDirPath+filePath);
        qDebug()<<"restored file"<<restoredPath;
        mArchiveFiles.append(restoredPath);
    }
    archiveFile.close();
    m_inited = true;
    return SE3_NO_ERROR;
}
