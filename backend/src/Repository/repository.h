#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../../include/MGit/mgit.h"
#include <git2.h>


class Repository : public IRepository{
private:
    git_repository *repo;
    QString path;
    QString token;
public:
    Repository() : repo(NULL), path(""), token("") {}
    ~Repository(){
        if(repo != NULL){
            git_repository_free(repo);
            repo = NULL;
        }
    }

    const QString& getPath() const {return path;}
    
    Gerror open(const QString& path_);

    void setToken(const QString& token_){token = token_;};


    Gerror gitClone(const QString& URL, const QString& username);
    Gerror gitFetch(const QString& username);
    Gerror gitPull();

    Gerror gitReset();
    Gerror gitStatus(QList<FileStatus>& list) const;
    Gerror gitLog(QList<CommitInfo>& list)const;

    bool isPathEmpty() const {return path.isEmpty();}
    bool hasRepo() const {return repo != NULL;}
};



#endif 
