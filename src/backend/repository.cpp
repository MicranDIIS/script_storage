#include "repository.h"
#include <QMetaType>

Repository::Repository(const RepoConfig& cfg) : repo_(NULL), cfg_(cfg){}

Repository::~Repository(){
    git_repository_free(repo_);
    repo_ = NULL;
}

IRepository* createRepository(const RepoConfig& cfg){
    git_libgit2_init();
    Repository* repo = new Repository(cfg);
    qRegisterMetaType<size_t>("size_t");

    return repo;
}

void deleteRepository(IRepository *repo){
    delete repo;
    git_libgit2_shutdown();
}


GitError libgitError(){
    const git_error* err = git_error_last();
    if(err && err -> message){
        QString error = QString::fromUtf8(err -> message);

        return GitError(error, err->klass);
    }

    return GitError("unknow", UNKNOW);
}
