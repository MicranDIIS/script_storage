#ifndef SETTINGREADER_H
#define SETTINGREADER_H

#include <QStringList>
#include <QString>
#include <QMap>

struct AppConfig { // структура для конфига
    QMap<QString, QString> scriptSource; // sourceFile -> путь, ключ значение
    QString allowExtension;
    bool recursive;

    QString prefix; // для шапки
    QString keySeparator;
    QString lineSeparator;
    QString valueSeparator;
};

struct HeaderRef { //струкутура для справочника
    QStringList remoteReqFields;
    QStringList localReqFields;

    QMap<QString, QStringList> allowedValues;
    QStringList textFields;
};

class SettingReader { // интерфейсный класс для загрузки справочника и конфига
public:
    virtual ~SettingReader(){}

    virtual bool configLoad(QString& path, AppConfig& ex_config)=0;
    virtual bool headerLoad(QString& path, HeaderRef& ref)=0;
};

class IniSettingReader: public SettingReader { // реализация интерфейса для ini файлов
public:
    virtual bool configLoad(QString& path, AppConfig& ex_config);
    virtual bool headerLoad(QString& path, HeaderRef& ref);
};


#endif // SETTINGREADER_H
