#include "../core/git_base.h"


int core_repo_init(const char *local_path)
{
    git_repository *repo = NULL;

    if(git_repository_init(&repo,local_path,0) != 0){
        core_errsys();
        return -1;
    }
    
    git_repository_free(repo);
    return 0;
}

git_repository *core_repo_connect(const char *local_path)
{
    git_repository *repo = NULL;

    if(git_repository_open(&repo,local_path) != 0){
        core_errsys();
        return NULL;
    }

    return repo;
}

int core_repo_clone(const char *URL,const char *local_path)
{
    git_repository *repo = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

    if(git_clone(&repo,URL,local_path,&clone_opts) != 0){
        core_errsys();
        return -1;
    }

    git_repository_free(repo);

    return 0;
}

//Релизация логики git fetch

__attribute__((visibility("hidden")))
int asdas(){
    return 13;
}