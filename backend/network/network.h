#ifndef NETWORK_H
#define NETWORK_H

#include <git2.h>
#include <basedata.h>
#include <QString>

int repo_clone(const QString &URL, const QString &path, const QString &username, const QString &token);

#endif
