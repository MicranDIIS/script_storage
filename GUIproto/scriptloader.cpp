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

//метод отслеживания файлов с одинаковыми метаданными - они оба будут отображаться в  гуи, но сначала будет окно с предупреждением о дублировании
//как идея потом добавить в отображение подсказку с уточняющими данными (название файла/путь)


