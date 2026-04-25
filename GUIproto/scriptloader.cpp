#include "scriptloader.h"

#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>



ScriptLoader::ScriptLoader(SettingReader *reader) : settingReader(reader){
}

bool ScriptLoader::configLoad(QString& path) {

    configPath = QDir::cleanPath(path);
    if (!settingReader){
        return false;
    }

    QString loadPath = configPath;
    return settingReader->configLoad(loadPath, ex_config);
}

bool ScriptLoader::headerLoad(QString& path) {

    headerPath = QDir::cleanPath(path);
    if (!settingReader){
        return false;
    }

    QString loadPath = headerPath;
    return settingReader->headerLoad(loadPath, ref);
}

// сканирование всех директорий
QList<FindFileInfo> ScriptLoader::scanSourcesAll() {
    QList<FindFileInfo> result;

    QMap<QString, QString>::const_iterator it;
    for (it = ex_config.scriptSource.constBegin();
         it != ex_config.scriptSource.constEnd(); ++it) {

        QString sourceKey = it.key();
        QString rawPath = it.value();
        QString absPath = convertPath(rawPath);

        if (absPath.isEmpty()) {
            qWarning() << "[ScriptLoader] path not resolve:" << sourceKey << "->" << rawPath;
            continue;
        }

        result.append(scanSourcesOne(sourceKey, absPath));
    }
    compareHeaderKey(result);
    return result;
}

//сканирование конкретной директории
QList<FindFileInfo> ScriptLoader::scanSourcesOne(QString& sourceFile, QString& rootPath) {

    QList<FindFileInfo> result;

    QDir dir(rootPath);
    if (!dir.exists()) {
        qWarning() << "[ScriptLoader] directory dont exist:" << rootPath;
        return result;
    }

    QDirIterator::IteratorFlags flags;

    if (ex_config.recursive) {
        flags = QDirIterator::Subdirectories;
    } else {
        flags = QDirIterator::NoIteratorFlags;
    }

    QDirIterator it(rootPath, QDir::Files | QDir::NoDotAndDotDot, flags); // итератор для файлов по флагу

    while (it.hasNext()) {
        QString filePath = it.next();

        if (!allowedExtension(filePath)) {
            continue;
        }

        QFileInfo fi(filePath);

        FindFileInfo info;
        info.sourceFile = sourceFile;
        info.absolutePath = fi.absoluteFilePath();
        info.relativePath = dir.relativeFilePath(fi.absoluteFilePath());
        info.fileName = fi.fileName();
        info.extension = fi.suffix();
        info.displayName.clear();

        headerParser.fullParse(info, ex_config, ref);

        result.append(info);
    }

    return result;
}

// проверка расширения файла
bool ScriptLoader::allowedExtension(QString& filePath) {
    if (ex_config.allowExtension.isEmpty()) {
        return true;
    }

    QFileInfo fi(filePath);
    return fi.suffix().compare(ex_config.allowExtension, Qt::CaseInsensitive) == 0;
}

// преобразование относительного пути в абсолютный из конфига
QString ScriptLoader::convertPath(QString& rawPath) {

    QFileInfo fi(rawPath);

    if (fi.isAbsolute()) {
        return QDir::cleanPath(rawPath);
    }

    QFileInfo cfgInfo(configPath);
    QString baseDir = cfgInfo.absoluteDir().absolutePath();

    QString resolved = QDir::cleanPath(baseDir + QDir::separator() + rawPath);
    return resolved;
}

//нормализация шапки для дальнейшего сравнения
QString ScriptLoader::makeHeaderKey(const FindFileInfo& info){
    QStringList parts;
    QMap<QString, QStringList>::const_iterator it;
    for (it = info.headerField.constBegin(); it != info.headerField.constEnd(); ++it){
        QString fieldName = it.key();
        QStringList values = it.value();

        QStringList headerKey;
        for (int i = 0; i < values.size(); i++){
            QString value = values.at(i);
            if(!value.isEmpty()){
                headerKey.append(value);
            }
        }
        headerKey.sort();
        parts.append(fieldName + "=" + headerKey.join(","));
    }
    parts.sort();
    return parts.join("|");
}

//метод для нахождения дубликатов, для разных скриптов с одинаковыми шапками приписывается имя файла
void ScriptLoader::compareHeaderKey(QList<FindFileInfo>& files){
   QList<FindFileInfo> result;
   QStringList addedScriptKey;

   QMap<QString, QList<int> > headerIndex;
   for(int i = 0; i < files.size(); i++){
       QString headerKey = makeHeaderKey(files.at(i));
       QString scriptKey = headerKey + "|file" + files.at(i).fileName;
       if (addedScriptKey.contains(scriptKey)) {
           qWarning() << "[ScriptLoader] duplicate script hidden:" << files.at(i).absolutePath;
           continue;
       }
       addedScriptKey.append(scriptKey);

       int newIndex = result.size();
       result.append(files.at(i));

       QList<int> indexes = headerIndex.value(headerKey);
       indexes.append(newIndex);
       headerIndex.insert(headerKey, indexes);
   }
   QMap<QString, QList<int> >::const_iterator it;
   for (it = headerIndex.constBegin(); it != headerIndex.constEnd(); ++it){
       QList<int> indexes = it.value();
       if (indexes.size() < 2) {//повторений нет, так как файл такой один
           continue;
       }
       for (int i = 0; i < indexes.size(); ++i) {
           int index = indexes.at(i);
           QString cleanDisplayName = result[index].displayName;
           result[index].displayName = cleanDisplayName + " [" + result[index].fileName + "]";
       }
   }
   files = result;
}






