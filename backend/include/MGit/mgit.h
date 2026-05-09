#ifndef MGIT_H
#define MGIT_H

#include <QString>
#include <QDateTime>
#include <QList>

//структура ошибки
struct Gerror{
    bool succses;
    QString msg;

    Gerror() : succses(true), msg("") {}

    Gerror(const QString& msg_) : succses(false) , msg(msg_) {}
};

//конфиг нашего репозитория
struct RepoConfig{
    //URL адрес удаленного репозитория(может лежать на гитхабе, гитлабе и тд)
    QString url;
    //ветка с которой будет идти работа
    QString branch;
    //путь где репозитория лежит либо куда будет клонировать
    QString path;
    //имя пользователя
    QString username;
    //токен пользователя
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

struct FileStatus{
    //путь относительно .git строится
    QString pathNew;
    QString pathOld;
    int flags;

    FileStatus(const QString& pathNew_, const QString& pathOld_, int flags_) : pathNew(pathNew_), pathOld(pathOld_), flags(flags_) {}
    bool flagCheck(STATUS_FLAG flag) const {return flags & flag;}
     
};

//для git log
struct CommitInfo{
    QDateTime authorDateTime;
    QString authorName;
    QString  authorEmail;

    QString commitMsg;
    QString commitHash;

    QDateTime committerDateTime;
    QString committerName;
    QString  committerEmail;

    CommitInfo(const QDateTime& authorDateTime_, const QString& authorName_,
               const QString& authorEmail_, const QString& commitMsg_,
               const QString& commitHash_, const QDateTime& committerDateTime_,
               const QString& committerName_, const QString& committerEmail_) :
               authorDateTime(authorDateTime_), authorName(authorName_),
               authorEmail(authorEmail_), commitMsg(commitMsg_),
               commitHash(commitHash_), committerDateTime(committerDateTime_),
               committerName(committerName_), committerEmail(committerEmail_) {}
};

//сам класс с виртуальными методами
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
    * открывает репозитрий на ветке из нашего конфига. Не работае если
    * ветки нет на локале, но есть в refs/remotes/origin.
    * только с локальными ветками которые УЖЕ ЕСТЬ
    */
    virtual Gerror open() = 0;

    /*
    * Клонирует весь репозиторий и сразу переключает на нужную ветку
    * В этом же случае, если ветка не main или master
    * То создается локальная ее копия с которой можно работать
    * После клонирования будет нужная нам ветка
    */
    virtual Gerror clone() = 0;
    /*
    * git fetch + git reset --hard origin/branch наш
    * git fetch фетчит ТОЛЬКО нашу ветку, а после уже применяет
    * git reset --hard origin/branch
    * ВАЖНО: файлы которые не отслеживаются не трогаются 
    */
    virtual Gerror sync() = 0;

    /*
    *git reset --hard HEAD~
    *ВАЖНО: ресетит все к ласт коммиту и файлы которые не отслеживаются тоже
    */
    virtual Gerror reset() = 0;
    /*
    *просто статус.
    *Флаги самим тыкать придется
    */
    virtual Gerror status(QList<FileStatus>& list) const = 0;
    /*
    *просто структуру с логами получаем
    */
    virtual Gerror log(QList<CommitInfo>& list) const = 0;

    virtual bool hasRepo() const = 0;

};

//фабричные ф-ии
IRepository* createRepository(const RepoConfig& cfg_);
void deleteRepository(IRepository* repo);

//запуск libgit2 через эти ф-ии производится. Просто что бы не тыкать libgit2 напрямую
void mgit_init();
void mgit_shutdown();

#endif 