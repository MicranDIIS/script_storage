#ifndef GIT_ERROR_H
#define GIT_ERROR_H

#include <repository.h>

GitError::GitError() : success(true), message(""), code(0) {}
GitError::GitError(const QString& message_, int code_) : success(false), message(message_), code(code_) {}


#endif
