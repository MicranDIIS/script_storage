#include "repository.h"


UpdateHandler::UpdateHandler(git_repository* repo,const QString& token,
                             const QString& url, const QString& username,
                             const QString& branch, const FileEventHandler& Filehandler,
                             QObject* parent) :
    QObject(parent), repo_(repo), token_(token), username_(username),
    url_(url), branch_(branch), Filehandler_(Filehandler){}

void UpdateHandler::callUpFetch(){
    emit handlerCallUpFetch();
}

