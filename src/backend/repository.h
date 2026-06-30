#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <mgit.h>
#include <git2.h>
#include <QObject>
#include <QTimer>
#include "updateHandler.h"

enum Errors{
    OK = 0,
    UNKNOW = 1,
    REPO_IS_NULL = 2,
    FILEHANDLER_IS_NOT_NULL = 3
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

class Repository : public QObject , public IRepository{
    Q_OBJECT
private:
    git_repository* repo_;
    RepoConfig cfg_;
    UpdateHandler* handler_;

    QTimer timer_;
public:
    Repository(const RepoConfig& cfg);
    ~Repository();

    const QString& getUrl() const;
    const QString& getBranch() const;
    const QString& getPath() const;
    const QString& getUsername() const;
    const QString& getToken() const;


    GitError open();

    GitError clone();
    GitError sync();

    GitError startCheckUpdatesActiveFile(const FileEventHandler& fileHandler,
                                     size_t timeSec);
    void stopCheckUpdatesActiveFile();

    GitError reset();
    GitError fillStatus(QList<FileStatus>& list) const;
    GitError fillLog(QList<CommitInfo>& list) const;
    GitError fillLog(QList<CommitInfo>& list, const QString& filePath) const;

    bool isValid() const;
private slots:
    GitError fetch();
};



//ф-ия получения ошибки из libgit2
GitError libgitError();

#endif

