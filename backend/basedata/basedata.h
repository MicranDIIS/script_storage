#ifndef BASEDATA_H
#define BASEDATA_H

#include <git2.h>
#include <QString>
#include <QList>


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
        QString path_new;
        QString path_old;
        int flags;
    public:
        //конструктор
        FileStatus(const QString &path_new_, const QString &path_old_, int flags_);
        //геттеры
        const QString& get_path_new(void)const;
        const QString& get_path_old(void)const;
        bool flag_check(STATUS_FLAG flag) const;
};


class Repo{
    private:
        git_repository *repo;
        QString path;
        QString token;
        QString error;
        explicit Repo(const QString &path,const QString &token_);

    public:
        //деконструктор
        ~Repo(void);

        //геттеры

        //возвращает путь
        const QString& get_path(void) const;
        //возвращает ошибку
        const QString& get_error(void) const;
        //возвращает токе
        const QString& get_token(void) const;

        //основное

        static Repo clone(const QString &URL,const QString &path,const QString &username,const QString &token);

        QList<FileStatus> status(void)const;

        int pull(void);

        int fetch(void);

        //вспомогательные ф-ии
        static Repo open(const QString &path, const QString &token);
        bool valid(void)const;
};

#endif
