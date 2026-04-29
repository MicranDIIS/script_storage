#ifndef COMMITINFO_H
#define COMMITINFO_H

#include <QDateTime>
#include <QString>

class CommitInfo{
    private:
        QDateTime author_date_time;
        QString   author_name;
        QString   author_email;

        QString   commit_msg;
        QString   commit_hash;

        QDateTime committer_date_time;
        QString   committer_name;
        QString   committer_email;

    public:
        CommitInfo(const QDateTime &author_date_time_,
                   const QString &author_name_,
                   const QString &author_email_,
                   const QString &commit_msg_,
                   const QString &commit_hash_,
                   const QDateTime &committer_date_time_,
                   const QString &committer_name_,
                   const QString &committer_email_)
            : author_date_time(author_date_time_)
            , author_name(author_name_)
            , author_email(author_email_)
            , commit_msg(commit_msg_)
            , commit_hash(commit_hash_)
            , committer_date_time(committer_date_time_)
            , committer_name(committer_name_)
            , committer_email(committer_email_) {}

        // Геттеры
        QDateTime get_author_date_time() const { return author_date_time; }
        QString   get_author_name() const { return author_name; }
        QString   get_author_email() const { return author_email; }

        QString   get_commit_msg() const { return commit_msg; }
        QString   get_commit_hash() const { return commit_hash; }

        QDateTime get_committer_date_time() const { return committer_date_time; }
        QString   get_committer_name() const { return committer_name; }
        QString   get_committer_email() const { return committer_email; }
};

#endif
