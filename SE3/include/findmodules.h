#ifndef FINDMODULES_H
#define FINDMODULES_H
#include "errormanager.h"
#include <QObject>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QTextCodec>

enum Paths{
    LIBS_PATH,
    PLUG_PATH,
    CORE_STORAGE_PATH,
    SCRIPTS_STORAGE_PATH,
    APP_PATH,
    BACKUP_PATH
};

class ModuleFinder : public QObject, public ErrorManager
{
    Q_OBJECT

public:
    ModuleFinder();
    ~ModuleFinder();
    int FindModule(QString name, QString* findedPath);
    QString GetPath(Paths path);
    QString GetNetworkPrefix(){return m_networkPrefix;}
    void SetNetworkPrefix(QString prefix){m_networkPrefix = prefix;}

private:
    QString m_libsRelPath;
    QString m_pluginsRelPath;
    QString m_coreStorageAbsPath;
    QString m_scriptsStorageAbsPath;
    QString m_networkPrefix;
    QFileInfo FindLastModifFile(QString path);
    void WaitCopyFileDone(QString filePath, qint64 size);
    QString m_absAppPath;
};

#endif // FINDMODULES_H
