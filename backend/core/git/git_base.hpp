#pragma once

#include <git2.h>
#include <QtCore/QtCore>

typedef struct git_t git_t;
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


