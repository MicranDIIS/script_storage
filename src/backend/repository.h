#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../../include/mgit.h"
#include <git2.h>
#include <QObject>
#include <QTimer>

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

class ContextUpdate;

class Repository : public IRepository{

private:
    git_repository* repo_;
    RepoConfig cfg_;
    ContextUpdate* contextObj_;
public:
    Repository(const RepoConfig& cfg);
    ~Repository();

    const QString& getUrl() const {return cfg_.url;}
    const QString& getBranch() const {return cfg_.branch;}
    const QString& getPath() const {return cfg_.path;}
    const QString& getUsername() const {return cfg_.username;}
    const QString& getToken() const {return cfg_.token;}


    GitError open();

    GitError clone();
    GitError fetch();
    GitError sync();

    void startCheckUpdatesActiveFile(const FileEventHandler& handler, size_t time, GitError& err);
    void checkUpdatesActiveFile(const FileEventHandler& handler, size_t time, GitError &err);
    void stopCheckUpdatesActiveFile();

    GitError reset();
    GitError fillStatus(QList<FileStatus>& list) const;
    GitError fillLog(QList<CommitInfo>& list) const;
    GitError fillLog(QList<CommitInfo>& list, const QString& filePath) const;

    bool isValid() const {return repo_ != NULL;}
};


class ContextUpdate : public QObject{
    Q_OBJECT
private:
    Repository* repo_;
    FileEventHandler handler_;
    size_t time_;
    GitError& err_;

    QTimer* timer_;
signals:
    void timeout();
private slots:
    void slotCheckUpdatesActiveFile(){repo_->checkUpdatesActiveFile(handler_, time_, err_);}

public:
    ContextUpdate(Repository* repo, const FileEventHandler& handler, size_t time, GitError& err):
                  repo_(repo), handler_(handler), time_(time), err_(err){
        timer_ = new QTimer();
        connect(timer_, SIGNAL(timeout()), this, SLOT(slotCheckUpdatesActiveFile()));
    }
    ~ContextUpdate(){if(timer_ != NULL) delete timer_;}
    void startCheckUpdatesActiveFile(){timer_->start(time_ * 1000);}
    void stopCheckActriveFile() {timer_->stop();}
};

//ф-ия получения ошибки из libgit2
GitError libgitError();

#endif

