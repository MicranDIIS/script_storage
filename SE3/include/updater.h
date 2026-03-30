#ifndef UPDATER_H
#define UPDATER_H
#include <QString>
#include <QList>
#include <QDir>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>
#include <QTextCodec>
#include "errormanager.h"
#include "sHead.h"

struct ModuleHistoryInfo
{
    QString hash;
    QString version;
    QString date;
    QString changes;
    QString creator;
};

struct ModuleInfo{
    QString name;
    QString path;
    QString link;
    QList<ModuleHistoryInfo> history;
    bool core;
    bool mandatory;
};

class Updater : public QObject, public ErrorManager
{
    Q_OBJECT

#ifdef TEST_MODE
    friend class Test_UpdaterTest;
#endif

public:
    Updater(const QString& targetRootPath = QCoreApplication::applicationDirPath());
    int ParseJSONFile(const QString& filePath);
    QByteArray CreateUpdateJSONData(QString appName, QList<ModuleInfo> mModulesList);

    short CheckForUpdate();
    bool IsUpdateAvailable(const QString& filePath);

    short CheckCoreConsist();
    short RestoreCoreConsist();

    short UpdateAll();
    int UpdateFile(const QString& filePath);

    QList<ModuleInfo> getModulesList();
    QList<ModuleInfo> getUpdateModulesList();
    QList<ModuleInfo> getCoreMissModulesList();

//    static QByteArray fileChecksum(const QString &fileName,
//                            QCryptographicHash::Algorithm hashAlgorithm)
//    {
//        QFile f(fileName);
//        if (f.open(QFile::ReadOnly)) {
//            QCryptographicHash hash(hashAlgorithm);
//            if (hash.addData(&f)) {
//                return hash.result();
//            }
//        }
//        return QByteArray();
//    }

    static QByteArray fileChecksum(const QString &fileName,
			    QCryptographicHash::Algorithm hashAlgorithm)
    {
	QFile f(fileName);
	if (f.open(QFile::ReadOnly)) {
	    QCryptographicHash hash(hashAlgorithm);

	    // В Qt 4 читаем файл порциями вручную
	    char buffer[8192];
	    qint64 bytesRead;
	    while ((bytesRead = f.read(buffer, sizeof(buffer))) > 0) {
		hash.addData(buffer, (int)bytesRead);
	    }

	    f.close();
	    return hash.result();
	}
	return QByteArray();
    }
    QString GetUpdateLink(const QString& filePath);

    static QByteArray dataChecksum(const QString &data,
                            QCryptographicHash::Algorithm hashAlgorithm)
    {
       return QCryptographicHash::hash(data.toLocal8Bit(),hashAlgorithm);
    }

private:
    QString mAppDestPath;
    QString mUpdateBasePath;
    QList<ModuleInfo> mModulesList;
    QList<ModuleInfo> mModulesForUpdateList;
    QList<QString> mPathsForUpdateList;
    QList<ModuleInfo> mModulesCoreMiss;
    int UpdateFile(ModuleInfo currModule);
};

#endif // UPDATER_H
