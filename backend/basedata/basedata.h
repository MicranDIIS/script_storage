#ifndef BASEDATA_H
#define BASEDATA_H

#include <git2.h>
#include <QString>
#include <QList>

//git status
struct FileStatus{
    QString path_new;
    QString path_old;
    int flags;

    FileStatus(const QString &path_new_, const QString &path_old_, int flags_) : path_new(path_new_), path_old(path_old_), flags(flags_){}
};

enum STATUS_FLAG{
    STATUS_NEW_TO_HEAD     = 1 << 0, //новый
    STATUS_MODFILE_TO_HEAD = 1 << 1, //изменен
    STATUS_DELETE_TO_HEAD  = 1 << 2, //удален
    STATUS_RENAME_TO_HEAD  = 1 << 3, //переименован
    STATUS_MODFILE_TO_DIR  = 1 << 4, //изменен в директории
    STATUS_DELETE_TO_DIR   = 1 << 5, //удален в директории
    STATUS_RENAME_TO_DIR   = 1 << 6, //переименован в директории
    STATUS_NEW_TO_DIR      = 1 << 7 //новый в директории
};
//конец git status

class Repo{
    private:
        git_repository *repo;
        QString path;
        QString token;
        QString error;
    public:
        //конструктор
        Repo(const QString &path);
        //деконструктор
        ~Repo(void);

        //get
        //возвращает путь
        const QString& get_path(void) const;
        //возвращает ошибку
        const QString& get_error(void) const;
        //возвращает токе
        const QString& get_token(void) const;

        //set
        void set_token(const QString &token_);

        //основное

        QList<FileStatus> status(void);

        int pull(void);

        int fetch(void);
};

#endif
