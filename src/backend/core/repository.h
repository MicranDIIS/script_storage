#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <mgit.h>
#include <git2.h>
#include <QObject>
#include <QTimer>
#include <git_raii.h>
#include <update_handler.h>
#include <git_utils.h>

enum Errors{
    OK = 0,
    UNKNOW = 1,
    REPO_IS_NULL = 2,
    FILEHANDLER_IS_NOT_NULL = 3
};

struct RepoConfigBuf{
    QByteArray url;
    QByteArray branch;
    QByteArray path;
    QByteArray username;
    QByteArray token;
    RepoConfigBuf(const RepoConfig& cfg);
};

class Repository : public QObject , public IRepository{
    Q_OBJECT
private:
    git_repository* repo_;
    RepoConfigBuf cfg_;
    UpdateHandler* handler_;

    QTimer timer_;

    Repository(const Repository&);
    Repository& operator=(const Repository&);

    GitError GitRevwalkInit(GitRevwalkPtr& walker) const;
    static const char* HEAD;
    static const char* ORIGIN;
public:
    explicit Repository(const RepoConfig& cfg);
    ~Repository();

    QString getUrl() const;
    QString getBranch() const;
    QString getPath() const;
    QString getUsername() const;
    QString getToken() const;
    GitError getTimeLastRemoteCommit(QTime &time) const;

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

    GitError startDebugMode();
    GitError saveDebugFiles(const QString &authorName, const QString &authorEmail, const QString &commitMsg);
    GitError mergeDebugFiles(const QString &filePath, const QString& authorName, const QString& authorEmail);
    GitError closeDebugMode();

    bool isValidRepo() const;
    bool isValid() const;
private slots:
    GitError fetch();
};

//ф-ия получения ошибки из libgit2
GitError libgitError();

#endif

