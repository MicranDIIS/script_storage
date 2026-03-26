#include "core/core.h"
#include <stdio.h>

#define local_path "/home/user/repository"

int main(void){
    git_libgit2_init();

    if(core_repo_init(local_path) != 0){
        core_errsys();
        printf("%s",errsys);
        return -1;
    }

    printf("Создан локальный репозиторий!\n");

    git_repository *repo = core_repo_connect(local_path);
    if(repo == NULL){
        core_errsys();
        printf("%s",errsys);
        return -1;
    }

    printf("Подключение с repo создано!");

    git_repository_free(repo);
    
    git_libgit2_shutdown();
    return 0;
}