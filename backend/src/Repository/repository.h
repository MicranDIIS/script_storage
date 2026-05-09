#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../../include/MGit/mgit.h"
#include <git2.h>


class Repository : public IRepository{
private:
    git_repository *repo;
    RepoConfig cfg;
    
    //ф-ии которые не нужны фронту, но нужны для логики бэкэнда
    Gerror fetch();
public:
    Repository(const RepoConfig& cfg_) : repo(NULL), cfg(cfg_) {}
    ~Repository(){if(repo != NULL){git_repository_free(repo);repo = NULL;}}

    const QString& getUrl() const {return cfg.url;}
    const QString& getBranch() const {return cfg.branch;}
    const QString& getPath() const {return cfg.path;}
    const QString& getUsername() const {return cfg.username;}
    const QString& getToken() const {return cfg.token;}

    Gerror open();

    Gerror clone();
    Gerror sync();

    Gerror reset();
    Gerror status(QList<FileStatus>& list) const;
    Gerror log(QList<CommitInfo>& list)const;

    bool hasRepo() const {return repo != NULL;}
};

//удобные оберкти для работы с ветками
int repo_remote_branch_create_to_local(git_repository* repo, const QString& branch);
int repo_checkout_local_branch(git_repository* repo, const QString& branch);

#endif 
