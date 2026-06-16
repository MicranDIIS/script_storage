#include "repository.h"

GitError::GitError() : success_(true), message_(""), code_(OK) {}
GitError::GitError(const QString& message, int code) : success_(false), message_(message), code_(code) {}
bool GitError::isError() const {return !success_;}
const QString& GitError::getMessage() const {return message_;}
int GitError::getCode() const {return code_;}

FileStatus::FileStatus(const QString& pathNew, const QString& pathOld, int flags) : pathNew_(pathNew), pathOld_(pathOld), flags_(flags) {}
const QString& FileStatus::getPathNew() const {return pathNew_;}
const QString& FileStatus::getPathOld() const {return pathOld_;}
bool FileStatus::isNewToHead() const {return flags_ & STATUS_NEW_TO_HEAD;}
bool FileStatus::isModFileToHead() const {return flags_ & STATUS_MODFILE_TO_HEAD;}
bool FileStatus::isDeleteToHead() const {return flags_ & STATUS_DELETE_TO_HEAD;}
bool FileStatus::isRenameToHead() const {return flags_ & STATUS_RENAME_TO_HEAD;}
bool FileStatus::isNewToDir() const {return flags_ & STATUS_NEW_TO_DIR;}
bool FileStatus::isModFileToDir() const {return flags_ & STATUS_MODFILE_TO_DIR;}
bool FileStatus::isDeleteToDir() const {return flags_ & STATUS_DELETE_TO_DIR;}
bool FileStatus::isRenameToDir() const {return flags_ & STATUS_RENAME_TO_DIR;}


CommitInfo::CommitInfo(const QDateTime& authorDate, const QString& authorName,
               const QString& authorEmail, const QString& commitMessage,
               const QString& commitHash, const QDateTime& committerDate,
               const QString& committerName, const QString& committerEmail) :
               authorDate_(authorDate), authorName_(authorName),
               authorEmail_(authorEmail), commitMessage_(commitMessage),
               commitHash_(commitHash), committerDate_(committerDate),
               committerName_(committerName), committerEmail_(committerEmail) {}
const QDateTime& CommitInfo::getAuthorDate() const {return authorDate_;}
const QString& CommitInfo::getAuthorName() const {return authorName_;}
const QString& CommitInfo::getAuthorEmail() const {return authorEmail_;}
const QString& CommitInfo::getCommitMessage() const {return commitMessage_;}
const QString& CommitInfo::getCommitHash() const {return commitHash_;}
const QDateTime& CommitInfo::getCommitterDate() const {return committerDate_;}
const QString& CommitInfo::getCommitterName() const {return committerName_;}
const QString& CommitInfo::getCommitterEmail() const {return committerEmail_;}
