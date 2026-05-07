#ifndef COMMITINFO_H
#define COMMITINFO_H

#include <QString>
#include <QDateTime>

struct CommitInfo
{
    QDateTime dateTime;
    QString author;
    QString authorEmail;
    QString commitMessage;
    QString commitHash;
};

#endif // COMMITINFO_H
