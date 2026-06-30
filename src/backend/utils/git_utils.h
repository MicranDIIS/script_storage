#ifndef GIT_UTILS_H
#define GIT_UTILS_H

#include <QString>
#include <git2.h>

struct GitData{
    QByteArray username;
    QByteArray token;

    GitData(const QString &username_,const QString &token_) : username(username_.toUtf8()) , token(token_.toUtf8()) {}
};

int callback(git_credential **out,const char *url,
                         const char *username_from_url,
                         unsigned int allowed_types,
                         void *payload);


struct LogFile {
    QString file_path;
    bool found;
};

int diff_file_callback(const git_diff_delta* delta, float progress, 
                              void* payload);
#endif