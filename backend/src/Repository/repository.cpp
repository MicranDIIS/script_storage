#include "repository.h"


IRepository* createRepository(const RepoConfig& cfg){
    git_libgit2_init();
    return new Repository(cfg);
}

void deleteRepository(IRepository *repo){
    delete repo;
    git_libgit2_shutdown();
}

Gerror libgitError(){
    const git_error* err = git_error_last();
    if(err && err -> message){
        QString error = QString::fromUtf8(err -> message);
        
        return Gerror(error, err->klass);
    }

    return Gerror("unknow", UNKNOW);
}
