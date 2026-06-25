#ifndef MGIT_H
#define MGIT_H

#include <QString>
#include <QDateTime>
#include <QList>

#if defined(MGIT_LIBRARY)
#  define MGITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MGITSHARED_EXPORT Q_DECL_IMPORT
#endif

struct MGITSHARED_EXPORT GitError{
    bool success;
    QString message;
    int code;

    GitError();
    GitError(const QString& message_, int code_);
};

//конфиг нашего репозитория
struct RepoConfig{
    QString url;
    QString branch;
    QString path;
    QString username;
    QString token;
};


struct FileStatus{
    QString pathNew;
    QString pathOld;

    bool newToHead; 
    bool modFileToHead;
    bool deleteToHead;
    bool renameToHead;

    bool newToDir;
    bool modFileToDir;
    bool deleteToDir;
    bool renameToDir;

};

typedef void (*callbackHandler) ();

struct FileEventHandler{
    callbackHandler callback;
    QString filePath;
};

struct CommitInfo{
    QString authorName;
    QString authorEmail;

    QString commitMessage;
    QString commitHash;
    QDateTime commitCreateTime;
};


class MGITSHARED_EXPORT IRepository{
public:
    virtual ~IRepository() {}

    //обычные геттеры
    virtual const QString& getUrl() const = 0;
    virtual const QString& getBranch() const = 0;
    virtual const QString& getPath() const = 0;
    virtual const QString& getUsername() const = 0;
    virtual const QString& getToken() const = 0;
    /*
    * Открывает репозиторий
    */
    virtual GitError open() = 0;

    /*
    * Клонирует только ветку заданную в конфиге
    */
    virtual GitError clone() = 0;
    /*
    * Фетчит ветку из конфига и ресетит до актуального состояния после фетча
    * Не трогает локальные файлы
    */
    virtual GitError sync() = 0;
    /*
    * Начинает в проверять наличие обновлений в определенном интервале
    */
    virtual void startCheckUpdatesActiveFile(const FileEventHandler& handler,size_t time, GitError& err) = 0;
    /*
    * Остановка проверки наличия обновлений
    */
    virtual void stopCheckUpdatesActiveFile() = 0;
    /*
    * Ресетит все к последнему коммиту. Локальные файлы удаляются
    */
    virtual GitError reset() = 0;
    /*
    * Статус файлов в индексе и локальные
    */
    virtual GitError fillStatus(QList<FileStatus>& list) const = 0;
    /*
    * Логи всех коммитов
    */
    virtual GitError fillLog(QList<CommitInfo>& list) const = 0;
    /*
    * Логи с коммитами в которых был изменен файл
    */
    virtual GitError fillLog(QList<CommitInfo>& list, const QString& filePath) const = 0;

    /*
    * Проверка валидности репозитория
    */
    virtual bool isValid() const = 0;

};

//фабричные ф-ии
extern "C"{
MGITSHARED_EXPORT IRepository* createRepository(const RepoConfig& cfg);
MGITSHARED_EXPORT void deleteRepository(IRepository* repo);
}

#endif 
