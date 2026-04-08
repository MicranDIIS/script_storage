#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include "settingreader.h"
#include <QString>
#include <QMap>
#include <QStringList>

struct FindFileInfo;

class HeaderParser { // отдельный класс для работы с шапкой
public:
    HeaderParser();
    void fullParse(FindFileInfo& info, AppConfig& ex_config, HeaderRef& ref);

private:
    void readHeader(FindFileInfo& info, AppConfig& ex_config);//чтение шапки из файла построчно
    void validateHeader(FindFileInfo& info, HeaderRef& ref);//проверка шапки по справочнику + поля для модели

    bool isHeaderLine(QString& line, AppConfig& ex_config);//проверка, что это шапка
    bool parseHeader(QString& line, QString& key, QStringList& values, AppConfig& ex_config);//парсер прочитанных строк шапки
    QStringList splitHeader(QString& value,  AppConfig& ex_config); // доп метод парсинга для разделения значениий и формирования списка
};

#endif // HEADERPARSER_H
