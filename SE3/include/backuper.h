#ifndef BACKUPER_H
#define BACKUPER_H
#include <QByteArray>
#include <QString>
#include <QStringList>
#include "errormanager.h"

#ifdef WIN32
#include <windows.h>
#else
#include <fcntl.h>
#endif

#define TITLE_BKUP_SCRIPTS "scriptSE3backUp"
#define TITLE_BKUP_MODULES "modulesSE3backUp"

#ifndef WIN32
struct InfoHeader{
    int pathSize;
    qint64 fileSize;
    qulonglong ftCreate;
    qulonglong ftAccess;
    qulonglong ftWrite;
};
#else
struct InfoHeader{
    int pathSize;
    qint64 fileSize;
    FILETIME ftCreate;
    FILETIME ftAccess;
    FILETIME ftWrite;
};
#endif

class BackUper: public QObject, public ErrorManager
{
public:
    BackUper(QString appDir, QString archivePath);

    /// Work with list of files
    short InitArchive(QString archivePath);
    short AddFile(QString relFilePath);
    short CreateBackUp(QString archivePath);
    short RestoreAllFromBackUp(QString archivePath);
    void ClearFiles();
    bool IsInited(){return m_inited;}

    short BackUp(QString path);

    QStringList GetAddedFileList();
    QStringList GetRestoredFileList();

    /// Work with single file
    short AppendFileToArchive(QString archivePath, QString relFilePath);
    short RestoreFileFromArchive(QString archivePath, QString relFilePath);
    bool CheckFileInArchive(QString fullPath);

private:
    short CreateArchiveArray(QString);
    bool CalcOffsetInArray(const QByteArray &archiveData, QString relFilePath, int &offset, int &len);
    short PrepareChunkOfData(QByteArray &readyData, QString relFilePath);
    short RestoreFromChunkOfData(const QByteArray &inputData, int& offset);
    short UpdateListOfFiles();

    QByteArray m_archive;
    QStringList m_files;
    QString mAppDirPath;
    QString mArchivePath;
    QStringList m_restored_files;
    QStringList mArchiveFiles;
    char m_header_mas[2048];
    bool m_inited;
};

#endif // BACKUPER_H
