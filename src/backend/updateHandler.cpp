#include "updateHandler.h"

UpdateHandler::UpdateHandler(git_repository* repo, const FileEventHandler& Filehandler,
                             const QString& url, const QString& token,
                             const QString& username, const QString& branch) :
    repo_(repo), Filehandler_(Filehandler), url_(url), token_(token),
    username_(username), branch_(branch) {}

