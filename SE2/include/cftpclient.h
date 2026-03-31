#ifndef CFTPCLIENT_H
#define CFTPCLIENT_H

#include <QObject>
#include <QFtp>
#include <QEventLoop>
#include <QFile>
#include <QBuffer>
#include <QTimer>
#include <QStringList>


/**
 * @brief Класс ftp-клиента.
 *
 * Представляет собой ftp-клиент с синхронизированными командами для использования в программах,
 * создаваемых с использованием библиотеки Qt.
 */
class CFtpClient : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса.
     *
     * @param ftpHost - ftp-хост.
     * @param ftpPort - порт для подключения к ftp-серверу.
     * @param ftpUser - имя пользователя на ftp-сервере.
     * @param ftpPassword - пароль на ftp-сервере.
     */
    CFtpClient(QString ftpHost, quint16 ftpPort, QString ftpUser, QString ftpPassword);

    /**
     * @brief Удаляет папку с ftp-сервера.
     *
     * @param dir - путь к папке на ftp-сервере, которую следует удалить.
     *
     * @return Возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool deleteDirFromFtp(QString dir);

    /**
     * @brief Удаляет файл с ftp-сервера.
     *
     * @param fileName- путь к файлу на ftp-сервере, который следует удалить.
     *
     * @return Возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool deleteFileFromFtp(QString fileName);

    /**
     * @brief Возвращает описание последней ошибки.
     *
     */
    QString errorString();

    /**
     * @brief Загружает файл с ftp-сервера.
     *
     * @param fileOnDisk - полный путь к файлу на диске. Если файл не существует, то он будет создан.
     * Если файл существует, то он будет перезаписан.
     * @param fileOnFtp - полный путь к файлу на ftp-сервере, который следует загрузить.
     *
     * @return Возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool getFile(QString fileOnDisk, QString fileOnFtp);

    /**
     * @brief Считывает содержание файла на ftp-сервере и передает его содержимое в строку.
     *
     * @param fileOnFtp - полный путь к файлу на ftp-сервере, содержимое которого следует считать.
     * @param str - указатель на строку, в которую будет передано содержимое файла.
     *
     * @return Возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool getTextFromFile(QString fileOnFtp, QString * str);

    /**
     * @brief Создает папку на ftp-сервере.
     *
     * @param dir - имя создаваемой папки, включающее полный путь к ней на ftp-сервере.
     *
     * @return Метод возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool makeDir(QString dir);

    /**
     * @brief Записывает файл или строку в папку или файл на ftp-сервере соответственно.
     *
     * @param fileOrString - путь к файлу, который следуюет записать, либо строка для записи.
     * @param dirOnFtp - в случае с записью файла: путь к папке на ftp-сервере, куда следует записать файл;
     * в случае с записью строки: путь к файлу на ftp-сервере. <b>Примечание.</b> При записи строки в файл,
     * последний должен существовать. При записи файла в папку существование папки не обязательно.
     *
     * @return Метод возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool writeFileToFtp(QString fileOrString, QString dirOnFtp);

    /**
     * @brief Записывает файл или строку в папку или файл на ftp-сервере соответственно.
     *
     * @param dirOnDisk - путь к папке, которую следует записать на ftp-сервер.
     * @param dirOnFtp - путь к папке на ftp-сервере, куда следует записать файлы из папки dirOnDisk;
     *
     * @return Метод возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool writeDirToFtp(QString dirOnDisk, QString dirOnFtp);

    /**
     * @brief Очищает корневые папки на ftp-сервере прибора.
     *
     * @return Метод возращает false в случае удачного выполнения и true - в случае ошибки.
     * Описание последней ошибки можно получить, вызвав метод CFtpClient::errorString();
     */
    bool clearFtp();



signals:
    /**
     * @brief Сигнал. Высылается по завершению следующих методов класса:
     * CFtpClient::deleteDirFromFtp(),
     * CFtpClient::deleteFileFromFtp(),
     * CFtpClient::getFile(),
     * CFtpClient::getTextFromFile(),
     * CFtpClient::makeDir(),
     * CFtpClient::writeFileToFtp().
     */
    void signalTransferComplete();

    void signalListComplete();
    void signalRemoveDirComplete();

public slots:

private slots:
    /**
     * @brief Слот. Выполняется проверка корректного выполнения ftp-командд. В случае неудачного выполнения
     * формирует сообщение об ошибке.
     *
     * @param idCommand - идентификатор команды
     * @param error - флаг ошибки. Если true - значит ошибка.
     */
    void slotCheckCommands(int idCommand, bool error);
    /**
     * @brief Слот. Срабатывает при достижении таймаута. Формирует сообщение об ошибке.
     *
     */
    void slotTimeOut();

    /**
     * @brief Слот. Срабатывает при выполнении команды list.
     *
     * @param url - содержит информацию об объекте в папке.
     */
    void slotGetContentList(QUrlInfo url);

private:
    /// Флаг ошибки.
    bool isError;

    bool isPutComplete;

    /// Идентификатор команды Put.
    int idPut;

    int idList;

    /// Идентификатор команды Remove.
    int idRemove;

    /// Идентификатор команды RemoveDir.
    int idRemoveDir;

    /// Идентификатор команды MakeDir.
    int idMakeDir;

    /// Идентификатор команды GetFile.
    int idGetFile;

    /// Идентификатор команды GetTextFromFile.
    int idGetTextFromFile;

    /// Время таймаута.
    int timeOutTime;

    /// Порт.
    quint16 port;

    /// Хост.
    QString host;

    /// Имя пользователя.
    QString user;

    /// Пароль.
    QString password;

    /// Содержание файла или строка с именем файла для записи.
    QString file;

    /// Папка на ftp куда следует записать файл.
    QString dir;

    /// Содержание файла при чтении с  ftp.
    QString strFromFile;

    /// Содержит текст ошибки.
    QString errorStr;

    /// Буффер для записи в него текста из файла на ftp.
    QBuffer * buffer;

    /// Объект файла.
    QFile * fileObject;

    /// Буффер для записи строки в файл на ftp.
    QByteArray * arr;

    /// Экземпляр QFtp
    QFtp * ftp;

    /// Таймер для таймаута.
    QTimer * timer;

    /// Список директорий в папке
    QStringList dirList;

    /// Список файлов в папке
    QStringList fileList;

    /// Текущая директория для которой выполняется list
    QString currentParentDir;


};

#endif // CFTPCLIENT_H
