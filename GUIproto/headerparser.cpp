#include "headerparser.h"
#include "scriptloader.h"

#include <QSettings>
#include <QFileInfo>
#include <QDebug>

HeaderParser::HeaderParser(){

}

void HeaderParser::fullParse(FindFileInfo& info, AppConfig& ex_config, HeaderRef& ref){

    info.headerExist = false;
    info.headerCorrect = false;
    info.headerField.clear();

    readHeader(info,ex_config);
    validateHeader(info,ref);
}

// проверка, что шапочка есть
bool HeaderParser::isHeaderLine(QString& line, AppConfig& ex_config) {

    QString trimmedLine = line.trimmed();
    if (trimmedLine.isEmpty()) {
        return false;
    }
    if (!trimmedLine.contains(ex_config.keySeparator)) {
            return false;
    }

    if (ex_config.lineSeparator.isEmpty()) {
            return true;
    }
    return trimmedLine.endsWith(ex_config.lineSeparator);
}

// доп метод для разделения значений
QStringList HeaderParser::splitHeader(QString& value, AppConfig& ex_config) {

    QStringList result;
    QString current = value.trimmed();

    if (current.isEmpty()) {
        return result;
    }

    if (ex_config.valueSeparator.isEmpty()) {
        result.append(current);
        return result;
    }

    // разрезаем строку по разделителю, игнорируя пустые элементы
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
bool HeaderParser::parseHeader(QString& line, FindFileInfo& info, AppConfig& ex_config) {

    QMap<QString, QStringList> fields; //временное хранилище 
    QString trimmedLine =line.trimmed();

    if (trimmedLine.isEmpty() || !trimmedLine.startsWith(ex_config.prefix)){
        return false;
    }
    //разделяем строку на части по ;
    QStringList parts = trimmedLine.split(ex_config.lineSeparator, QString::SkipEmptyParts);
    for (int i=0; i < parts.size(); ++i){

        QString part = parts.at(i).trimmed();
        if (part.startsWith(ex_config.prefix)) {
            part = part.mid(ex_config.prefix.length()).trimmed(); // удаляем префикс, если он есть, в каждой паре ключ-значение
        }

        int sepPos = part.indexOf(ex_config.keySeparator);//ищем : в парах
        if (sepPos == -1){
//           qDebug() << "[HeaderParser::parseHeader] keySeparator not found in part:" << part << " script:"  << info.fileName;
           return false;
        }

        QString key = part.left(sepPos).trimmed().toLower(); // то, что слева от : это ключ
        QString valueText = part.mid(sepPos + ex_config.keySeparator.length()).trimmed(); // то, что справа от : это значение
        QStringList values = splitHeader(valueText, ex_config); // разделяем на список, если несколько значений
        fields.insert(key, values);
    }

    QMap<QString, QStringList>::const_iterator it;
    for (it = fields.constBegin(); it != fields.constEnd(); ++it) {
        info.headerField.insert(it.key(), it.value());
    }
    return true;
}

//чтение "сырых" метаданных из шапки
void HeaderParser::readHeader(FindFileInfo& info, AppConfig& ex_config) {

    QFile file(info.absolutePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    bool headerStarted = false;
    QString headerText;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QString trimmedLine = line.trimmed();

        if (!headerStarted && trimmedLine.isEmpty()) {
            continue;
        }

        if (!headerStarted) {
            if (!isHeaderLine(line, ex_config)) {
                return;
            }
            headerStarted = true;
            info.headerExist = true;
        }

        if (!isHeaderLine(line, ex_config)) {
//            qDebug() << "[HeaderParser::readHeader] header end on line:" << trimmedLine << " script:"  << info.fileName;
            break;
        }

        headerText += trimmedLine;
    }

    if (!headerText.isEmpty()) {
        if (!parseHeader(headerText, info, ex_config)) {
            info.headerExist = false;
        }
    }
}

//валидация по справочнику и формироание полей для будущей модели отображения
void HeaderParser::validateHeader(FindFileInfo& info, HeaderRef& ref) {

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
