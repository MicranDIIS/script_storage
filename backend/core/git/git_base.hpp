#pragma once

#include <git2.h>
#include <QtCore/QtCore>

typedef struct git_t git_t;

git_repository *git_get_repo(git_t *gt);

/*
    делает git clone на локалку
    **
    const QString &URL - URL репозитория который клонируем
    const QString &path - путь клонирования на локалке
    const QString &token - токен для доступа к репозитрию
    **
    в случае ошибки вернет -1, иначе 0
*/
int core_git_clone(const QString &URL,const QString &path,const QString &token);


/*
    открывает локальный репозиторий, но наличии инициализации гита
    **
    const QString &path - путь к репозиторию
    **
    в случае ошибки вернет nullptr, иначе git_t *
*/
git_t *core_git_open(const QString &path);

/*
    делает git status 
    **
    git_repository *repo - репозиторий статус которого будем получать
    **
    в случае ошибки вернет пустой QList<QString>, иначе имена измененных файлов 
*/
QList<QString> core_git_status(git_repository *repo);

/*
    выполняет git pull для обновления локального репозитория
    **
    git_t *gt - структура с открытым репозиторием
    const QString &token - токен для доступа к репозиторию (опционально)
    **
    в случае ошибки вернет -1, иначе 0
*/
int core_git_pull(git_t *gt, const QString &token = QString());