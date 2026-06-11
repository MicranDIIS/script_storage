#include "repository.h"

GitError::GitError() : success_(true), msg_(""), code_(OK) {}
GitError::GitError(const QString& msg, int code) : success_(false), msg_(msg), code_(code) {}
bool GitError::isError() const {return !success_;}
const QString& GitError::getMsg() const {return msg_;}
int GitError::getCode() const {return code_;}

FileStatus::FileStatus(const QString& pathNew, const QString& pathOld, int flags) : pathNew_(pathNew), pathOld_(pathOld), flags_(flags) {}
const QString& FileStatus::getPathNew() const {return pathNew_;}
const QString& FileStatus::getPathOld() const {return pathOld_;}
bool FileStatus::isNewToHead() const {return flags_ & STATUS_NEW_TO_HEAD;}
bool FileStatus::isModfileToHead() const {return flags_ & STATUS_MODFILE_TO_HEAD;}
bool FileStatus::isDeleteToHead() const {return flags_ & STATUS_DELETE_TO_HEAD;}
bool FileStatus::isRenameToHead() const {return flags_ & STATUS_RENAME_TO_HEAD;}
bool FileStatus::isNewToDir() const {return flags_ & STATUS_NEW_TO_DIR;}
bool FileStatus::isModfileToDir() const {return flags_ & STATUS_MODFILE_TO_DIR;}
bool FileStatus::isDeleteToDir() const {return flags_ & STATUS_DELETE_TO_DIR;}
bool FileStatus::isRenameToDir() const {return flags_ & STATUS_RENAME_TO_DIR;}


CommitInfo::CommitInfo(const QDateTime& authorDateTime, const QString& authorName,
               const QString& authorEmail, const QString& commitMsg,
               const QString& commitHash, const QDateTime& committerDateTime,
               const QString& committerName, const QString& committerEmail) :
               authorDateTime_(authorDateTime), authorName_(authorName),
               authorEmail_(authorEmail), commitMsg_(commitMsg),
               commitHash_(commitHash), committerDateTime_(committerDateTime),
               committerName_(committerName), committerEmail_(committerEmail) {}
const QDateTime& CommitInfo::getAuthorDateTime() const {return authorDateTime_;}
const QString& CommitInfo::getAuthorName() const {return authorName_;}
const QString& CommitInfo::getAuthorEmail() const {return authorEmail_;}
const QString& CommitInfo::getCommitMsg() const {return commitMsg_;}
const QString& CommitInfo::getCommitHash() const {return commitHash_;}
const QDateTime& CommitInfo::getCommitterDateTime() const {return committerDateTime_;}
const QString& CommitInfo::getCommitterName() const {return committerName_;}
const QString& CommitInfo::getCommitterEmail() const {return committerEmail_;}