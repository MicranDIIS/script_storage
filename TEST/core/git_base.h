#ifndef GIT_BASE_H
#define GIT_BASE_H


#ifdef __cplusplus
extern "C"{
#endif


#include "git_err.h"

/*
    инициализрует локальный репозиторий по адресу const char*
    в случае ошибки возвращает -1, иначе 0
*/
int core_repo_init(const char*);


/*
    осуществляет подключение к локальному репозиторию
    в случае ошибки возвращает NULL,иначе указатель на git_repository
*/
git_repository *core_repo_connect(const char*);


/*
    первый аргумент URL,второая path.
    клонирует удаленный репозиторий по пути path 
    в случае ошибки возвращает -1,иначе 0
*/
int core_repo_clone(const char*,const char*);

/*
    аналогично git fecth скачивает новые данные из удаленного репозитория
    но не сливает их с локальной веткой
    в случае ошибки возвращает -1, иначе 0
*/
int core_repo_fetch(const char*,const char*);

#ifdef __cplusplus 
}
#endif

#endif