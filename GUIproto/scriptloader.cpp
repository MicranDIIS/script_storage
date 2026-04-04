#include "scriptloader.h"

#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>

// ПОКА ТОЛЬКО ОБХОД ДИРЕКТОРИЙ  И ФИЛЬТРАЦИЯ ПО РАЗРЕШЕНИЮ ФАЙЛА. ВЫВОДИТСЯ В MAINWINDOW ВСЕ ЦЕЛИКОМ В БАЗОВОМ РЕЖИМЕ. МОЖНО ЧЕРЕЗ ОТКЛАДКУ ПОСМОТРЕТЬ, КАК ОБХОДИТ ФАЙЛЫ.
//// КОНФИГОМ ВЫСТУПАЕТ ФАЙЛ app_config.ini - не стала пока брать json, так как пока не очень разобралась как с ними работать в qt 4, нужно будет отдельную библиотеку подключать видимо ?
// ВСЕ ОТНОСИТЕЛЬНЫЕ ПУТИ СЧИТАЮТСЯ ОТНОСИТЕЛЬНО ПАПКИ, ГДЕ ЛЕЖИТ КОНФИГ

//        [script_source]
//        local = ../scripts/scripts_local
//        remote = ../scripts/scripts_remote
//        presets = ../scripts/presets

//        [scan]
//        allowExtension=lua
//        recursive=true

// Я СЕЙЧАС РАБОТАЮ С ТАКОЙ СТРУКТУРОЙ ФС

//config/
//       app_config.ini
//scripts/
//        scripts_local/
//        scripts_presets/
//        scripts_remote/
//src/ тут код

ScriptLoader::ScriptLoader(){}

// загрузка конфига из .ini
bool ScriptLoader::configLoad(QString& path) {

    configPath = QDir::cleanPath(path);

    ex_config.scriptSource.clear();
    ex_config.allowExtension.clear();

    QFileInfo cfgFile(configPath);
    if (!cfgFile.exists() || !cfgFile.isReadable()) {
        return false;
    }

    QSettings settings(configPath, QSettings::IniFormat);

    settings.beginGroup("script_source");
    QStringList keys = settings.childKeys();

    //qDebug() << "[ScriptLoader] script_source keys =" << keys;

    for (int i = 0; i < keys.size(); ++i) {
        QString key = keys.at(i);
        QString value = settings.value(key).toString().trimmed();

        if (!key.isEmpty() && !value.isEmpty()) {
            ex_config.scriptSource.insert(key, value);
        }
    }
    settings.endGroup();

    settings.beginGroup("scan");
    ex_config.allowExtension = settings.value("allowExtension").toString().trimmed();
    ex_config.recursive = settings.value("recursive").toBool();
    settings.endGroup();

//    qDebug() << "[ScriptLoader] configPath =" << configPath;
//    qDebug() << "[ScriptLoader] Config loaded. Sources:" << ex_config.scriptSource
//             << "| Extension:" << ex_config.allowExtension
//             << "| Recursive:" << ex_config.recursive;

    return !ex_config.scriptSource.isEmpty();
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

        result.append(info);
    }

//    qDebug() << "[ScriptLoader] Source:" << sourceFile
//             << "| Found files:" << result.size()
//             << "in" << rootPath;

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

//    qDebug() << "[ScriptLoader] rawPath =" << rawPath;
//    qDebug() << "[ScriptLoader] ConfigPath =" << configPath;

    QFileInfo cfgInfo(configPath);
    QString baseDir = cfgInfo.absoluteDir().absolutePath();

//    qDebug() << "[ScriptLoader] cfg absolute dir =" << baseDir;

    QString resolved = QDir::cleanPath(baseDir + QDir::separator() + rawPath);

//    qDebug() << "[ScriptLoader] resolved =" << resolved;

    return resolved;
}
