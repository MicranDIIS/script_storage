#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../../import/mgit.h"
#include <git2.h>


class Repository : public IRepository{
private:
    git_repository *repo_;
    RepoConfig cfg_;
    
    Gerror fetch();
public:
    Repository(const RepoConfig& cfg) : repo_(NULL), cfg_(cfg) {}
    ~Repository(){if(repo_ != NULL){git_repository_free(repo_);repo_ = NULL;}}

    const QString& getUrl() const {return cfg_.url;}
    const QString& getBranch() const {return cfg_.branch;}
    const QString& getPath() const {return cfg_.path;}
    const QString& getUsername() const {return cfg_.username;}
    const QString& getToken() const {return cfg_.token;}

    Gerror open();

    Gerror clone();
    Gerror sync();

    Gerror reset();
    Gerror status(QList<FileStatus>& list) const;
    Gerror log(QList<CommitInfo>& list) const;
    Gerror log(QList<CommitInfo>& list, const QString& filePath) const;

    bool hasRepo() const {return repo_ != NULL;}
};

//ф-ия получения ошибки из libgit2
Gerror libgitError();

#endif 

