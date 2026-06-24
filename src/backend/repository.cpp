#include "repository.h"

Repository::Repository(const RepoConfig& cfg){
    repo_ = NULL;
    cfg_ = cfg;
    func_ = NULL;

    connect(this, SIGNAL(signalCheckUpdatesActiveFile(size_t,QString)),
            this, SLOT(slotCheckUpdatesActiveFile(size_t,QString)),
            Qt::QueuedConnection);
}

Repository::~Repository(){
    git_repository_free(repo_);
    repo_ = NULL;
}

void Repository::run(){

    exec();
}

IRepository* createRepository(const RepoConfig& cfg){
    git_libgit2_init();
    Repository* repo = new Repository(cfg);

    repo->moveToThread(repo);

    repo->start();

    return repo;
}

void deleteRepository(IRepository *repo){
    Repository* repo_ = static_cast<Repository*>(repo);
    repo_->quit();
    repo_->wait();
    delete repo_;
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
