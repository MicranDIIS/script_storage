#include "repository.h"

GitError::GitError() : success(true), message(""), code(OK) {}
GitError::GitError(const QString& message_, int code_) : success(false), message(message_), code(code_) {}
