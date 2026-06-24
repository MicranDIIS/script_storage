#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../../include/mgit.h"
#include <git2.h>
#include <QThread>

enum Errors{
    OK = 0,
    UNKNOW = 1,
    REPO_IS_NULL = 2
};

enum STATUS_FLAG{
    STATUS_NEW_TO_HEAD     = 1 << 0,
    STATUS_MODFILE_TO_HEAD = 1 << 1,
    STATUS_DELETE_TO_HEAD  = 1 << 2,
    STATUS_RENAME_TO_HEAD  = 1 << 3, 
    STATUS_MODFILE_TO_DIR  = 1 << 4,
    STATUS_DELETE_TO_DIR   = 1 << 5,
    STATUS_RENAME_TO_DIR   = 1 << 6,
    STATUS_NEW_TO_DIR      = 1 << 7  
};


class Repository : public QThread, public IRepository{
    Q_OBJECT
private:
    git_repository* repo_;
    RepoConfig cfg_;
    callback_t func_;

    void checkUpdates();
    GitError fetch();
public:
    Repository(const RepoConfig& cfg);
    ~Repository();

    const QString& getUrl() const {return cfg_.url;}
    const QString& getBranch() const {return cfg_.branch;}
    const QString& getPath() const {return cfg_.path;}
    const QString& getUsername() const {return cfg_.username;}
    const QString& getToken() const {return cfg_.token;}

    void setCallbackNotification(callback_t func){func_ = func;}

    GitError open();

    GitError clone();
    GitError sync();
    void startCheckUpdatesActiveFile(size_t time, const QString& filePath);

    GitError reset();
    GitError fillStatus(QList<FileStatus>& list) const;
    GitError fillLog(QList<CommitInfo>& list) const;
    GitError fillLog(QList<CommitInfo>& list, const QString& filePath) const;

    bool isValid() const {return repo_ != NULL;}
protected:
    void run();
signals:
    void signalCheckUpdatesActiveFile(size_t time, const QString& filePath);
private slots:
    void slotCheckUpdatesActiveFile(size_t time, const QString& filePath);
};

//ф-ия получения ошибки из libgit2
GitError libgitError();

#endif 

