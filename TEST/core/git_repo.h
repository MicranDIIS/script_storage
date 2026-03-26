#ifndef GIT_REPO_H
#define GIT_REPO_H


#ifdef __cplusplus
extern "C" {
#endif


#include "git_base.h"

/*
    аналогично git commit -m "что-то"
    1 - const char* путь к локальному репозиторию
    2- const char* имя того кто делает коммит
    3 - const char* почта типа кто делал коммит
    4 - сообщение коммита
    в случае ошибки возвращает -1, иначе 0
    но тут могут быть системный ошибки, поэтому следует смотреть и errscus
*/
int core_repo_commit(git_repository*,const char*,const char*,const char*);


/*
    аналогично git push. пушит на удаленный репозиторий
    git_repository - указатель на репозиторий в который пушим
    int - кол-во аргументов для полного path
    char** - path
    в случае ошибки возвращает -1,иначе 0
*/
int core_repo_push(git_repository*,int,char*[]);




#ifdef __cplusplus
}
#endif

#endif