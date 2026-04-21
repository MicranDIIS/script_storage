#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <basedata.h>

struct CommitInfo {
    QDateTime dateTime;
    QString author;
    QString authorEmail;
    QString commitMessage;
    QString commitHash;
};

struct RepoConfig {
    QString url;
    QString branch;
    QString path;
    QString username;
    QString token;
};

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool clone(const RepoConfig &config) = 0;

    virtual bool pull(const RepoConfig &config) = 0;

    virtual QList<FileStatus> getStatus() = 0;

    virtual QList<CommitInfo> getFileHistory(const QString &filePath) = 0;
};


IRepository* createRepository();
void destroyRepository(IRepository *repo);

#endif 