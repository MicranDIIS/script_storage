#ifndef MGIT_H
#define MGIT_H

#include <QString>
#include <QDateTime>
#include <QList>

//коды ошибок связанных с пользователем 
enum Errors{
    OK = 0,
    UNKNOW = 1,
    REPO_IS_NULL = 2
};

//класс ошибки
class Gerror{
private:
    bool success_;
    QString msg_;
    int code_;
public:
    Gerror() : success_(true), msg_(""), code_(OK) {}
    Gerror(const QString& msg, int code) : success_(false), msg_(msg), code_(code) {}

    bool hasError() const {return !success_;}
    bool matchesCode(int code) {return code_ == code;}

    const QString& getMsg() const {return msg_;}
    int getCode() const {return code_;}
};

//конфиг нашего репозитория
struct RepoConfig{
    QString url;
    QString branch;
    QString path;
    QString username;
    QString token;
};

//для git status
enum STATUS_FLAG{
    STATUS_NEW_TO_HEAD     = 1 << 0, //новый
    STATUS_MODFILE_TO_HEAD = 1 << 1, //изменен
    STATUS_DELETE_TO_HEAD  = 1 << 2, //удален
    STATUS_RENAME_TO_HEAD  = 1 << 3, //переименован
    STATUS_MODFILE_TO_DIR  = 1 << 4, //изменен в директории
    STATUS_DELETE_TO_DIR   = 1 << 5, //удален в директории
    STATUS_RENAME_TO_DIR   = 1 << 6, //переименован в директории
    STATUS_NEW_TO_DIR      = 1 << 7  //новый в директории
};

class FileStatus{
private:
    QString pathNew_;
    QString pathOld_;
    int flags_;

public:
    FileStatus(const QString& pathNew, const QString& pathOld, int flags) : pathNew_(pathNew), pathOld_(pathOld), flags_(flags) {}

    const QString& getPathNew() const {return pathNew_;}
    const QString& getPathOld() const {return pathOld_;}

    bool statusNewToHead() const {return flags_ & STATUS_NEW_TO_HEAD;}
    bool statusModfileToHead() const {return flags_ & STATUS_MODFILE_TO_HEAD;}
    bool statusDeleteToHead() const {return flags_ & STATUS_DELETE_TO_HEAD;}
    bool statusRenameToHead() const {return flags_ & STATUS_RENAME_TO_HEAD;}

    bool statusNewToDir() const {return flags_ & STATUS_NEW_TO_DIR;}
    bool statusModfileToDir() const {return flags_ & STATUS_MODFILE_TO_DIR;}
    bool statusDeleteToDir() const {return flags_ & STATUS_DELETE_TO_DIR;}
    bool statusRenameToDir() const {return flags_ & STATUS_RENAME_TO_DIR;}

    bool flagCheck(STATUS_FLAG flag) const {return flags_ & flag;}
};

//для git log
class CommitInfo{
private:
    QDateTime authorDateTime_;
    QString authorName_;
    QString authorEmail_;

    QString commitMsg_;
    QString commitHash_;

    QDateTime committerDateTime_;
    QString committerName_;
    QString committerEmail_;

public:
    CommitInfo(const QDateTime& authorDateTime, const QString& authorName,
               const QString& authorEmail, const QString& commitMsg,
               const QString& commitHash, const QDateTime& committerDateTime,
               const QString& committerName, const QString& committerEmail) :
               authorDateTime_(authorDateTime), authorName_(authorName),
               authorEmail_(authorEmail), commitMsg_(commitMsg),
               commitHash_(commitHash), committerDateTime_(committerDateTime),
               committerName_(committerName), committerEmail_(committerEmail) {}

    const QDateTime& getAuthorDateTime() const {return authorDateTime_;}
    const QString& getAuthorName() const {return authorName_;}
    const QString& getAuthorEmail() const {return authorEmail_;}

    const QString& getCommitMsg() const {return commitMsg_;}
    const QString& getCommitHash() const {return commitHash_;}

    const QDateTime& getCommitterDateTime() const {return committerDateTime_;}
    const QString& getCommitterName() const {return committerName_;}
    const QString& getCommitterEmail() const {return committerEmail_;}

};

class IRepository{
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
    virtual Gerror open() = 0;

    /*
    * Клонирует только ветку заданную в конфиге
    */
    virtual Gerror clone() = 0;
    /*
    * Фетчит ветку из конфига и применяет ресетит до актуального фетча
    * Не трогает локальные файле
    */
    virtual Gerror sync() = 0;

    /*
    * Ресетит все к ласт коммиту. Локальные файлы удаляются
    */
    virtual Gerror reset() = 0;
    /*
    * Статус файлов в индексе и локальные
    */
    virtual Gerror status(QList<FileStatus>& list) const = 0;
    /*
    * Логи всех коммитов
    */
    virtual Gerror log(QList<CommitInfo>& list) const = 0;
    /*
    * Логи с коммитами в которых был изменен файл
    */
    virtual Gerror log(QList<CommitInfo>& list, const QString& filePath) const = 0;

    /*
    * Проверка валидности репозитория
    */
    virtual bool hasRepo() const = 0;

};

//фабричные ф-ии
IRepository* createRepository(const RepoConfig& cfg);
void deleteRepository(IRepository* repo);


#endif