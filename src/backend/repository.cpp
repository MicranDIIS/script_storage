#include "repository.h"
#include <QMetaType>

Repository::Repository(const RepoConfig& cfg) : repo_(NULL), cfg_(cfg), handler_(NULL){
    connect(this, SIGNAL(callUpFetch()), this, SLOT(slotCallUpFetch()));
}

Repository::~Repository(){
    git_repository_free(repo_);
    repo_ = NULL;
}

const QString& Repository::getUrl() const {return cfg_.url;}
const QString& Repository::getBranch() const {return cfg_.branch;}
const QString& Repository::getPath() const {return cfg_.path;}
const QString& Repository::getUsername() const {return cfg_.username;}
const QString& Repository::getToken() const {return cfg_.token;}
bool Repository::isValid() const {return repo_ != NULL;}

void Repository::slotCallUpFetch(){fetch();}

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
