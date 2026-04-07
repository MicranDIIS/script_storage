#include "scriptloader.h"

#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>

// ДОБАВИЛА К ОБХОДУ ПАРСИНГ ШАПКИ, ПРОВЕРКУ ПО СПРАВОЧНИКУ header_ref.ini И ФОРМИРОВАНИЕ ПРИМЕРНЫХ ПОЛЕЙ ДЛЯ БУДУЩЕЙ МОДЕЛИ ОТОБРАЖЕНИЯ
// ВЫНЕСЛА ЛОГИКУ РАБОТЫ С ini ФАЙЛАМИ В ОТДЕЛЬНЫЙ ИНТЕРФЕЙС SettingReader
// КОНФИГОМ ВЫСТУПАЕТ ФАЙЛ app_config.ini. ФАЙЛЫ ЛЕЖАТ В ПАПКЕ config, ее тоже запушила
// ВСЕ ОТНОСИТЕЛЬНЫЕ ПУТИ СЧИТАЮТСЯ ОТНОСИТЕЛЬНО ПАПКИ, ГДЕ ЛЕЖИТ КОНФИГ

// СТРУКТУРА ФС
//config/
//       app_config.ini
//       header_ref.ini
//scripts/
//        scripts_local/
//        scripts_presets/
//        scripts_remote/
//src/ тут код

// ВИД ШАПКИ
// для локальных скриптов шапочка такая же как была, только надо дописывать -- перед ключом,и в целом использовать разделители из конфига
// для скриптов из репозитория шапка такая: регистр любой можно использовать, пробелы тоже 

// --roles:developer,production;
// --devices:G7M-06;
// --stades:calibration;
// --specific: Auto IQ Correction (check); 

// пока нормальной модели нет, отображение происходит в базовом режиме всех скриптов с валидными шапочками, название - specific

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
        info.headerExist = false;
        info.headerCorrect = false;
        info.sourceFile = sourceFile;
        info.absolutePath = fi.absoluteFilePath();
        info.relativePath = dir.relativeFilePath(fi.absoluteFilePath());
        info.fileName = fi.fileName();
        info.extension = fi.suffix();
        info.displayName.clear();

        readHeader(info);
        validateHeader(info);


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

// проверка, что шапочка есть
bool ScriptLoader::isHeaderLine(QString& line) {

    QString trimmedLine = line.trimmed();
    if (trimmedLine.isEmpty()) {
        return false;
    }
    if (!trimmedLine.startsWith(ex_config.prefix)) {
        return false;
    }

    QString content = trimmedLine.mid(ex_config.prefix.length()).trimmed(); // убрать префикс
    if (!content.contains(ex_config.keySeparator)) {
        return false;
    }
    if (!ex_config.lineSeparator.isEmpty() && !content.endsWith(ex_config.lineSeparator)) {
        return false;
    }
    return true;
}

// доп метод для разделения значений
QStringList ScriptLoader::splitHeader(QString& value) {

    QStringList result;
    QString current = value.trimmed();

    if (current.isEmpty()) {
        return result;
    }

    if (!ex_config.lineSeparator.isEmpty() && current.endsWith(ex_config.lineSeparator)) {
        current.chop(ex_config.lineSeparator.length());
        current = current.trimmed();
    }

    if (ex_config.valueSeparator.isEmpty()) {
        result.append(current);
        return result;
    }

    QStringList rawValues = current.split(ex_config.valueSeparator, QString::SkipEmptyParts);

    for (int i = 0; i < rawValues.size(); ++i) {
        QString item = rawValues.at(i).trimmed();

        if (!item.isEmpty()) {
            result.append(item);
        }
    }

    return result;
}

//парсинг прочитанной шапки
bool ScriptLoader::parseHeader(QString& line, QString& key, QStringList& values) {

    key.clear();
    values.clear();

    if (!isHeaderLine(line)) {
        return false;
    }
    QString trimmedLine = line.trimmed();
    QString content = line.mid(ex_config.prefix.length()).trimmed();

    if (!ex_config.lineSeparator.isEmpty() && content.endsWith(ex_config.lineSeparator)) {
        content.chop(ex_config.lineSeparator.length());
        content = content.trimmed();
    }

    int sepPos = content.indexOf(ex_config.keySeparator);
    key = content.left(sepPos).trimmed().toLower();
    QString valueText = content.mid(sepPos + ex_config.keySeparator.length()).trimmed();

    if (key.isEmpty()) {
        return false;
    }

    values = splitHeader(valueText);
    return true;
}

//чтение "сырых" метаданных из шапки 
void ScriptLoader::readHeader(FindFileInfo& info) {

    QFile file(info.absolutePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    bool headerStarted = false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QString trimmedLine = line.trimmed();

        if (!headerStarted && trimmedLine.isEmpty()) {
            continue;
        }

        if (!headerStarted) {
            if (!isHeaderLine(line)) { //определение является ли первая непустая строка началом шапки
                return;
            }
            headerStarted = true;
            info.headerExist = true;
        }

        if (!isHeaderLine(line)) {
            break;
        }

        QString key;
        QStringList values;
        if (!parseHeader(line, key, values)) {
            return;
        }

        // if (info.headerField.contains(key)){
        //     return;
        // }

        info.headerField.insert(key, values); // тут логика такая, что не может быть повторяющихся ключей в шапке                    
    }
}

//валидация по справочнику и формироание полей для будущей модели отображения
void ScriptLoader::validateHeader(FindFileInfo& info) {

    if (!info.headerExist || info.headerField.isEmpty()) {
        return;
    }

    QStringList requiredFields;

    if (info.sourceFile == "remote") { // в зависимости от источника скрипта (разные шапки)
        requiredFields = ref.remoteReqFields;
    } else if (info.sourceFile == "local") {
        requiredFields = ref.localReqFields;
    } else {
        return;
    }

    for (int i = 0; i < requiredFields.size(); ++i) {
        QString field = requiredFields.at(i).trimmed().toLower();

        if (!info.headerField.contains(field)) {
            return;
        }
        if (info.headerField.value(field).isEmpty()) {
            return;
        }
    }

    QMap<QString, QStringList>::const_iterator it;
    for (it = info.headerField.constBegin(); 
         it != info.headerField.constEnd(); ++it) {

        QString fieldName = it.key();
        QStringList values = it.value();

        if (ref.textFields.contains(fieldName.trimmed().toLower())) {
            continue;
        }

        QStringList allowed = ref.allowedValues.value(fieldName);
        for (int i = 0; i < values.size(); ++i) {
            QString value = values.at(i).trimmed();
            if (!allowed.contains(value)) { 
                return;
            }
        }
    }

    //перенос проверенных значений шапки в поля структуры
    if (info.headerField.contains("roles")) {
        info.roles = info.headerField.value("roles");
    }

    if (info.headerField.contains("devices")) {
        info.devices = info.headerField.value("devices");
    }

    if (info.headerField.contains("stades")) {
        info.stades = info.headerField.value("stades");
    }

    if (info.headerField.contains("specific") && !info.headerField.value("specific").isEmpty()) {
        info.specific = info.headerField.value("specific").first();
        info.displayName = info.specific; // specific как название скрипта 
    }

    if (info.headerField.contains("categories") && !info.headerField.value("categories").isEmpty()) {
        info.categories = info.headerField.value("categories").first();
    }

    info.headerCorrect = true;
}
