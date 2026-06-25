#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <mgit.h>
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

class TimerHelper;
class UpdateHandler;

class Repository : public QObject, public IRepository{
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
    GitError fetch();
    GitError sync();

    void startCheckUpdatesActiveFile(const FileEventHandler& fileHandler,
                                     size_t time);
    void stopCheckUpdatesActiveFile();

    GitError reset();
    GitError fillStatus(QList<FileStatus>& list) const;
    GitError fillLog(QList<CommitInfo>& list) const;
    GitError fillLog(QList<CommitInfo>& list, const QString& filePath) const;

    bool isValid() const;
signals:
    void callUpFetch();
private slots:
    void slotCallUpFetch();
    void slotCallUpCheckUpdateActiveFile();
};

class UpdateHandler : public QObject{
    Q_OBJECT
private:
    git_repository* repo_;
    QString token_;
    QString username_;
    QString url_;
    QString branch_;
    FileEventHandler Filehandler_;

public:
    UpdateHandler(git_repository* repo, const QString& token,
                  const QString& username, const QString& url,
                  const QString& branch, const FileEventHandler& handler,
                  QObject* parent = NULL);
    void callUpFetch();
    void checkUpdatesActiveFile();

signals:
    void handlerCallUpFetch();
};


//ф-ия получения ошибки из libgit2
GitError libgitError();

#endif

