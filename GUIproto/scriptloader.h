#ifndef SCRIPTLOADER_H
#define SCRIPTLOADER_H

#include "settingreader.h"
#include "headerparser.h"
#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>


struct FindFileInfo { // структура с информацией о скриптах для модели
    QString sourceFile; // local, remote, preset
    QString absolutePath;
    QString relativePath;
    QString fileName;
    QString extension;

    QMap<QString, QStringList> headerField; // поля шапки
    QStringList roles;
    QStringList devices;
    QStringList stades;
    QString specific;
    QString categories;

    bool headerExist; // логика для проверки шапки
    bool headerCorrect;

    QString displayName; // поля для названия в отображении

};

class ScriptLoader {

public:

    ScriptLoader(SettingReader *reader); 

    QList<FindFileInfo> scanSourcesAll(); // сканирование dвсей директории скриптов
    QList<FindFileInfo> scanSourcesOne(QString& sourceFile, QString& rootPath); // сканирование одной директории

    bool configLoad(QString& path);
    bool headerLoad(QString& path);
    QString makeHeaderKey(const FindFileInfo& info);
    void compareHeaderKey(QList<FindFileInfo>& files);


private:
    SettingReader *settingReader;
    HeaderParser headerParser;

    AppConfig ex_config;
    HeaderRef ref;

    QString configPath;
    QString headerPath;

    bool allowedExtension(QString& filePath);//проверка корректности расширения
    QString convertPath (QString& rawPath);//преобразование пути из относительного в абсолютный
};

#endif // SCRIPTLOADER_H
