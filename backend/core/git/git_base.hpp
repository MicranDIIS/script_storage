#pragma once

#include <git2.h>
#include <QtCore/QtCore>

//данная структура нужна для реализации ф-ии status
struct FileStatus{
    QString new_path;
    QString old_path;
    int flags;
    
    FileStatus(const QString &new_path_,const QString &old_path_,int flags_) : new_path(new_path_) , old_path(old_path_) , flags(flags_) {}
};

class Repo_git{
    private:
        git_repository *repo;
        QString path;
        QString token;
        QString error = "";
    public:
        //конструктор
        Repo_git(const QString &path);
        //деконструктор
        ~Repo_git();

        //методы

        //get
        //возвращает указатель на git_репозиторий
        git_repository *get_repo();
        //возвращает путь к git_репозиторию
        QString get_path();
        //возвращает последнюю ошибку
        QString get_error();

        //основное

        /*
            делает git status 
            **
            в случае ошибки вернет пустой QList<QString> и запишет в error,
            иначе QList. Где элементы это структура FileStatus с информацией 
        */
        QList<FileStatus> status();

        /*
            делает git pull
            **
            в случае ошибки вернет -1 и запишет в error, иначе 0  
        */
        int pull(const QString &username, const QString &token);

        /*
            делает git fetch
            **
            в случае ошибки вернет -1 и запишет в error, иначе 0
        */
        int fetch();
};


//свободные функции(было бы криво их пихать в методы класса)


/*
    
*/
Repo_git core_git_init(const QString &path);

/*
    делает git clone на локалку
    **
    const QString &URL - URL репозитория который клонируем
    const QString &path - путь клонирования на локалке
    const QString &token - токен для доступа к репозитрию
    **
    в случае ошибки вернет -1, иначе 0
*/
int core_git_clone(const QString &URL,const QString &path,
                   const QString &username,const QString &token);
