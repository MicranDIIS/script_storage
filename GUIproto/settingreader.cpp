#include "settingreader.h"

#include <QSettings>
#include <QFileInfo>
#include<QDebug>

bool IniSettingReader::configLoad(QString& path, AppConfig& ex_config){ //загрузка конфига из .ini, логика та же что и в прошлой версии
    ex_config = AppConfig();

    QFileInfo cfgFile(path);
    if (!cfgFile.exists() || !cfgFile.isReadable()) {
        return false;
    }

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("script_source");
    QStringList keys = settings.childKeys();
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

    settings.beginGroup("header");
    ex_config.prefix = settings.value("prefix").toString().trimmed();
    ex_config.keySeparator = settings.value("keySeparator").toString().trimmed();
    ex_config.lineSeparator = settings.value("lineSeparator").toString().trimmed();
    ex_config.valueSeparator = settings.value("valueSeparator").toString().trimmed();
    settings.endGroup();

//    qDebug() << "prefix =" << ex_config.prefix;
//    qDebug() << "keySeparator =" << ex_config.keySeparator;
//    qDebug() << "valueSeparator =" << ex_config.valueSeparator;
//    qDebug() << "lineSeparator =" << ex_config.lineSeparator;

    return !ex_config.scriptSource.isEmpty();
}

bool IniSettingReader::headerLoad(QString& path, HeaderRef& ref) { // загрузка справочника из .ini

    ref = HeaderRef();

    QFileInfo refFile(path);
    if (!refFile.exists() || !refFile.isReadable()) {
        return false;
    }

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("local"); 
    QStringList localFields = settings.value("requiredfields").toStringList();
    for (int i = 0; i < localFields.size(); ++i) {
        QString field = localFields.at(i).trimmed().toLower();
        if (!field.isEmpty()) {
            ref.localReqFields.append(field);
        }
    }
    settings.endGroup();

    settings.beginGroup("remote");
    QStringList remoteFields = settings.value("requiredfields").toStringList();
    for (int i = 0; i < remoteFields.size(); ++i) {
        QString field = remoteFields.at(i).trimmed().toLower();
        if (!field.isEmpty()) {
            ref.remoteReqFields.append(field);
        }
    }
    settings.endGroup();

    QStringList groups = settings.childGroups();
    for (int i = 0; i < groups.size(); ++i) {
        QString group = groups.at(i).trimmed();
        QString groupLower = group.toLower();

        if (groupLower == "local" || groupLower == "remote") {
            continue;
        }

        settings.beginGroup(group);
        if (settings.contains("values")) {
            QStringList values = settings.value("values").toStringList();
            QStringList normalValues;
            for (int j = 0; j < values.size(); ++j) {
                QString value = values.at(j).trimmed();
                if (!value.isEmpty()) {
                    normalValues.append(value);
                }
            }
            ref.allowedValues.insert(groupLower, normalValues);
        }
        if (settings.contains("text") && settings.value("text").toBool()) {
            ref.textFields.append(groupLower);
        }
        settings.endGroup();
    }
    return true;
}
