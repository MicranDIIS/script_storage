#include "updater.h"
#include <QJsonDocument.h>
#include <QJsonObject.h>
#include <QJsonArray.h>
#include<QJsonParseError.h>
#include <QFile>
#include <QDebug>

Updater::Updater(const QString& targetRootPath):ErrorManager(false)
{
    mAppDestPath = targetRootPath;
}

/*!
    \brief Парсим JSON файл

    Инициализируем список модулей
 */
int Updater::ParseJSONFile(const QString &filePath)
{
     return SE3_NO_ERROR;
    mModulesList.clear();
    qDebug()<<"parse json"<<filePath;
    QFile updateInfo(filePath);
    if(filePath.isEmpty() || !updateInfo.open(QIODevice::ReadOnly)){
        return SetError(/*UPD_DBFILE_MISS*/filePath+" is incorrect");
    }

    QFileInfo fiUpdateInfo(filePath);
    mUpdateBasePath = fiUpdateInfo.absoluteDir().absolutePath();
    if(mUpdateBasePath.at(mUpdateBasePath.length()-1)!='/')
        mUpdateBasePath.append("/");

    QJsonParseError jErr;
    QJsonDocument document = QJsonDocument::fromJson(updateInfo.readAll(),&jErr);
    if(jErr.error > 0)
        return SetError(tr("JSON parse error: %1").arg(jErr.errorString()));

    QJsonObject root = document.object();

    QString firstKey = root.keys().at(0);
    if(firstKey!="appName"){
        return SetError("Wrong file format");;
    }

    QJsonValue jvModules = root.value("modules");
    if(!jvModules.isArray()){
        return SetError("Can't find section 'modules'");
    }

    QJsonArray jaModules = jvModules.toArray();
    for(int i = 0; i < jaModules.count(); i++){
        QJsonObject joModulesSubtree = jaModules.at(i).toObject();
        QJsonValue jvHistory = joModulesSubtree.value("history");
        ModuleInfo modInfo;
        modInfo.name = joModulesSubtree.value("name").toString();
        modInfo.path = joModulesSubtree.value("path").toString();
        modInfo.link = joModulesSubtree.value("link").toString();
        modInfo.mandatory = joModulesSubtree.value("mandatory").toBool(false);
        modInfo.core = joModulesSubtree.value("core").toBool(false);
        if(!jvHistory.isArray()){
            return SetError(tr("Can't find section 'modules/history' for module %1").arg(modInfo.name));
        }

        QJsonArray jaHistory = jvHistory.toArray();
        QList<ModuleHistoryInfo> modHistoryList;
        for(int i = 0; i < jaHistory.count(); i++){
            ModuleHistoryInfo modHistoryInfo;
            QJsonObject joHistorySubtree = jaHistory.at(i).toObject();
            modHistoryInfo.hash = joHistorySubtree.value("hash").toString();
            modHistoryInfo.version = joHistorySubtree.value("version").toString();
            modHistoryInfo.date = joHistorySubtree.value("date").toString();
            modHistoryInfo.creator = joHistorySubtree.value("creator").toString();
            modHistoryInfo.changes = joHistorySubtree.value("changes").toString();
            modHistoryList.append(modHistoryInfo);
        }
        modInfo.history = modHistoryList;
        mModulesList.append(modInfo);
    }
    return SE3_NO_ERROR;
}

/*!
    \brief Проверка на наличие обновлений

    Проверяем по времени модификации и по хешу
    \return Количество модулей требующих обновления
 */
short Updater::CheckForUpdate()
{
    mModulesForUpdateList.clear();
    mPathsForUpdateList.clear();
    for(int i=0; i<mModulesList.length(); i++)
    {
        QString modName = mModulesList.at(i).name;
        QString currFilePath = mAppDestPath+QDir::separator()+mModulesList.at(i).path+QDir::separator()+modName;

//        qDebug()<<"check update: currFilePath:"<<currFilePath;
        QFileInfo testFile(currFilePath);
        if(!testFile.exists())  // если файла нет в базе обновлений - пропускаем
            continue;

        QDateTime lastModifTestFile = testFile.lastModified();
//        qDebug()<<"check update: lastModifTestFile:"<<lastModifTestFile.toString("dd.MM.yyyy hh:mm:ss");
        QByteArray testHash;
        QString data;
        stDecodeData metaData;
        QDateTime lastModifUpdBaseFile;
        if(Reader::unpackFile(testFile.absoluteFilePath(),data,metaData))
        {
            testHash = Updater::dataChecksum(data,QCryptographicHash::Md5).toHex();
            if(metaData.scriptChangeTime.isValid() && metaData.scriptChangeTime.date().year()!=1970){
                lastModifTestFile = metaData.scriptChangeTime;
            }
        }
        else{
            testHash = Updater::fileChecksum(testFile.absoluteFilePath(),QCryptographicHash::Md5).toHex();
        }
//        qDebug()<<"check update: lastModifTestFile2:"<<lastModifTestFile.toString("dd.MM.yyyy hh:mm:ss");
        int historyLength = mModulesList.at(i).history.length();
        bool dublicate = false;
        QString lastUpdHash="";
        if(historyLength)
        {
            lastModifUpdBaseFile = QDateTime::fromString(mModulesList.at(i).history.at(0).date,"dd.MM.yyyy hh:mm:ss");
            lastUpdHash = mModulesList.at(i).history.at(0).hash;
            for(int j=0; j<historyLength; j++)
            {
                QDateTime currModifUpdBaseFile = QDateTime::fromString(mModulesList.at(i).history.at(j).date,"dd.MM.yyyy hh:mm:ss");

                if(lastModifUpdBaseFile < currModifUpdBaseFile)
                {
                   lastModifUpdBaseFile = currModifUpdBaseFile;
                   lastUpdHash = mModulesList.at(i).history.at(j).hash;
                }
//                if(testHash == mModulesList.at(i).history.at(j).hash)
//                    dublicate = true;
            }
            if(testHash == lastUpdHash)
                dublicate = true;
        }
//        qDebug()<<"check update: dublicate:"<<dublicate;
//        qDebug()<<"check update: last hash:"<<lastUpdHash;
//        qDebug()<<"check update: lastModifUpdBaseFile:"<<lastModifUpdBaseFile.toString("dd.MM.yyyy hh:mm:ss");
        if(historyLength && testHash!=lastUpdHash && !dublicate)
        {
            if(lastModifTestFile<lastModifUpdBaseFile)
            {
                mModulesForUpdateList.append(mModulesList.at(i));
                mPathsForUpdateList.append(testFile.absoluteFilePath());
            }
        }
    }
    return mModulesForUpdateList.length();
}

bool Updater::IsUpdateAvailable(const QString& filePath)
{
    return mPathsForUpdateList.contains(filePath);
}

/*!
    \brief Проверка на целостность файлов - "ядра" (минимально необходимых)

    Проверяем по тэгу "core" в списке модулей
    \return Количество пропущенных модулей
 */
short Updater::CheckCoreConsist()
{
    mModulesCoreMiss.clear();
    for(int i=0; i<mModulesList.length(); i++)
    {
        QString modName = mModulesList.at(i).name;
        QString filePath = mAppDestPath + QDir::separator() +
                mModulesList.at(i).path + QDir::separator() +
                modName;
        QFileInfo testFile(filePath);
        if(mModulesList.at(i).core && !testFile.exists()){
            mModulesCoreMiss.append(mModulesList.at(i));
        }
    }
    return mModulesCoreMiss.length();
}

/*!
    \brief Восстановление целостности ядра

    Копирование недостающих файлов из хранилища.
    \return Код ошибки.
 */
short Updater::RestoreCoreConsist()
{
    QDir dir;
    for(int i=0; i<mModulesCoreMiss.length(); i++)
    {
        ModuleInfo currModule = mModulesCoreMiss.at(i);
        QString currSourceFile = mUpdateBasePath + QDir::separator() + currModule.path + QDir::separator() + currModule.name + QDir::separator() + currModule.history.last().hash;
        QString currTargetPath = mAppDestPath + QDir::separator() + currModule.path + QDir::separator();
        QString currTargetFile = currTargetPath + currModule.name;
        if(!dir.mkpath(currTargetPath))
            return SetError(tr("Can`t create directory %1").arg(currTargetPath).arg(currTargetFile));
        QFile sourceFile(currSourceFile);
        if(!currModule.link.isEmpty())
        {
            QFileInfo tfi(currTargetFile);
            QFileInfo lfi(mAppDestPath + QDir::separator() + currModule.link);
//            qDebug()<<"link"<<tfi.absoluteFilePath()<<lfi.absoluteFilePath();
            QFile::link(lfi.absoluteFilePath(),tfi.absoluteFilePath());
        }
        else{
            if(!sourceFile.copy(currTargetFile)){
                qDebug()<<sourceFile.errorString();
//                mModulesCoreMiss.removeAt(i);
                return SetError(tr("Can`t restore file from %1 to %2").arg(currSourceFile).arg(currTargetFile));
            }
        }
    }
    return SE3_NO_ERROR;
}

/*!
    \brief Полное обновление

    Копирование обновлённых файлов из хранилища.
    \return Код ошибки.
 */
short Updater::UpdateAll()
{
    int res = SE3_NO_ERROR;
    for(int i=0; i<mModulesForUpdateList.length(); i++)
    {
        ModuleInfo currModule = mModulesForUpdateList.at(i);
        int res = UpdateFile(currModule);
        if(res)
            break;
    }
    return res;
}

int Updater::UpdateFile(const QString& filePath)
{
    int index = mPathsForUpdateList.indexOf(filePath);
    if(index == -1)
        return SetError(UPD_FILE_NONEED);

    int updRes = SE3_NO_ERROR;
    if(index>=0)
    {
        updRes = UpdateFile(mModulesForUpdateList.at(index));
        if(!updRes){
            mPathsForUpdateList.removeAt(index);
            mModulesForUpdateList.removeAt(index);
        }
    }
    return updRes;
}

int Updater::UpdateFile(ModuleInfo currModule)
{
    QString currSourceFile = mUpdateBasePath + QDir::separator() +
            currModule.path + QDir::separator() +
            currModule.name + QDir::separator() +
            currModule.history.last().hash;
    QString currTargetPath = mAppDestPath + QDir::separator() +
            currModule.path + QDir::separator();
    QString currTargetFile = currTargetPath + currModule.name;
    QDir dir;
    dir.mkpath(currTargetPath);
    QFile updFile(currTargetFile);
    QFileInfo targetInfo(currTargetFile);
    QFileInfo sourceInfo(currSourceFile);
    if(updFile.exists() && !updFile.remove())
        return SetError(tr("Can`t remove file %1").arg(currTargetFile));

    if(!currModule.link.isEmpty())
    {
        QFileInfo tfi(currTargetFile);
        QFileInfo lfi(mAppDestPath + QDir::separator() + currModule.link);
        QFile::link(lfi.absoluteFilePath(),tfi.absoluteFilePath());
    }
    else{
        if(!QFile::copy(sourceInfo.absoluteFilePath(), targetInfo.absoluteFilePath()))
            return SetError(tr("Can`t update file from %1 to %2").arg(sourceInfo.absoluteFilePath()).arg(targetInfo.absoluteFilePath()));
    }
    return SE3_NO_ERROR;
}

/*!
    \brief Создаём файл с историей обновлений

    Формат - JSON
    \return Текстовое содержимое файла.
 */
QByteArray Updater::CreateUpdateJSONData(QString appName, QList<ModuleInfo> mModulesList)
{
    QJsonObject rootObj;
    rootObj.insert("appName",QJsonValue::fromVariant(appName));

    QJsonArray modulesArray;

    for(int i=0;i<mModulesList.length();i++)
    {
        QJsonObject moduleObj;
        ModuleInfo moduleInfo = mModulesList.at(i);
        moduleObj.insert("name",QJsonValue::fromVariant(moduleInfo.name));
        moduleObj.insert("path",QJsonValue::fromVariant(moduleInfo.path));
        moduleObj.insert("mandatory",QJsonValue::fromVariant(moduleInfo.mandatory));
        moduleObj.insert("core",QJsonValue::fromVariant(moduleInfo.core));
        moduleObj.insert("link",QJsonValue::fromVariant(moduleInfo.link));

        QJsonArray historyArray;
        for(int j=0;j<moduleInfo.history.length();j++)
        {
            QJsonObject historyObject;
            ModuleHistoryInfo moduleHistory = moduleInfo.history.at(j);
            historyObject.insert("hash",QJsonValue::fromVariant(moduleHistory.hash));
            historyObject.insert("version",QJsonValue::fromVariant(moduleHistory.version));
            historyObject.insert("changes",QJsonValue::fromVariant(moduleHistory.changes));
            historyObject.insert("date",QJsonValue::fromVariant(moduleHistory.date));
            historyObject.insert("creator",QJsonValue::fromVariant(moduleHistory.creator));
            historyArray.push_back(historyObject);
        }
        moduleObj.insert("history",historyArray);
        modulesArray.push_back(moduleObj);
    }
    rootObj.insert("modules",modulesArray);

    QJsonDocument doc(rootObj);
    return doc.toJson();
}

QList<ModuleInfo> Updater::getModulesList()
{
    return mModulesList;
}

QList<ModuleInfo> Updater::getUpdateModulesList()
{
    return mModulesForUpdateList;
}

QList<ModuleInfo> Updater::getCoreMissModulesList()
{
    return mModulesCoreMiss;
}

QString Updater::GetUpdateLink(const QString &filePath)
{
    int index = mPathsForUpdateList.indexOf(filePath);

    if(index == -1){
        SetError(UPD_FILE_NONEED);
        return "";
    }

    ModuleInfo currModule = mModulesForUpdateList.at(index);
    return mUpdateBasePath + QDir::separator() + currModule.path + QDir::separator() + currModule.name + QDir::separator() + currModule.history.last().hash;
}
