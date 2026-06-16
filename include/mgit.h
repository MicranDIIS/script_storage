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

class MGITSHARED_EXPORT GitError{
private:
    bool success_;
    QString message_;
    int code_;
public:
    GitError();
    GitError(const QString& message, int code);

    bool isError() const;

    const QString& getMessage() const;
    int getCode() const;
};

//конфиг нашего репозитория
struct RepoConfig{
    QString url;
    QString branch;
    QString path;
    QString username;
    QString token;
};


class MGITSHARED_EXPORT FileStatus{
private:
    QString pathNew_;
    QString pathOld_;
    int flags_;

public:
    FileStatus(const QString& pathNew, const QString& pathOld, int flags);

    const QString& getPathNew() const;
    const QString& getPathOld() const;

    bool isNewToHead() const;
    bool isModFileToHead() const;
    bool isDeleteToHead() const;
    bool isRenameToHead() const;

    bool isNewToDir() const;
    bool isModFileToDir() const;
    bool isDeleteToDir() const;
    bool isRenameToDir() const;
};

//для git log
class MGITSHARED_EXPORT CommitInfo{
private:
    QDateTime authorDate_;
    QString authorName_;
    QString authorEmail_;

    QString commitMessage_;
    QString commitHash_;

    QDateTime committerDate_;
    QString committerName_;
    QString committerEmail_;

public:
    CommitInfo(const QDateTime& authorDate, const QString& authorName,
               const QString& authorEmail, const QString& commitMessage,
               const QString& commitHash, const QDateTime& committerDate,
               const QString& committerName, const QString& committerEmail);

    const QDateTime& getAuthorDate() const;
    const QString& getAuthorName() const;
    const QString& getAuthorEmail() const;

    const QString& getCommitMessage() const;
    const QString& getCommitHash() const;

    const QDateTime& getCommitterDate() const;
    const QString& getCommitterName() const;
    const QString& getCommitterEmail() const;

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
