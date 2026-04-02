#ifndef SCRIPTLOADER_H
#define SCRIPTLOADER_H

#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>


struct FindFileInfo {
    QString sourceFile; // local, remote, preset
    QString absolutePath;
    QString relativePath;
    QString fileName;
    QString extension;
};


struct AppConfig {
    QMap<QString, QString> scriptSource; // sourceFile -> путь, ключ значение
    QString allowExtension;
    bool recursive;
};

class ScriptLoader {

public:

    ScriptLoader();
    QList<FindFileInfo> scanSourcesAll(); // сканирование dвсей директории скриптов
    bool configLoad (QString& configPath); // проверка чтения конфига

private:
    AppConfig ex_config;
    QString configPath;

    QList<FindFileInfo> scanSourcesOne(QString& sourceFile, QString& rootPath); // сканирование одной директории
    bool allowedExtension(QString& filePath);//проверка корректности расширения
    QString convertPath (QString& rawPath);//преобразование пути из относительного в абсолютный
};

#endif // SCRIPTLOADER_H
