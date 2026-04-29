#ifndef IREPOSITORYFACTROY_H
#define IREPOSITORYFACTROY_H

#include <IRepository.h>
#include <QString>

IRepository *repo_open(const QString& path, const QString& URL, const QString& token);
bool repo_init(const QString& path);
bool repo_is_valid(IRepository* repo);
void repo_delete(IRepository* repo);

#endif
