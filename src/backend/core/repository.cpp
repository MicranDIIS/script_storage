#include "repository.h"
#include <QMetaType>

Repository::Repository(const RepoConfig& cfg) : repo_(NULL), cfg_(cfg), handler_(NULL){}

Repository::~Repository(){
    git_repository_free(repo_);
    repo_ = NULL;
}

QString Repository::getUrl() const {return QString::fromUtf8(cfg_.url);}
QString Repository::getBranch() const {return QString::fromUtf8(cfg_.branch);}
QString Repository::getPath() const {return QString::fromUtf8(cfg_.path);}
QString Repository::getUsername() const {return QString::fromUtf8(cfg_.username);}
QString Repository::getToken() const {return QString::fromUtf8(cfg_.token);}
bool Repository::isValid() const {return repo_ != NULL;}

const char* Repository::HEAD = "HEAD";
const char* Repository::ORIGIN = "origin";

GitError Repository::GitRevwalkInit(GitRevwalkPtr& walker) const{
    walker.reset();
    if (git_revwalk_new(&walker, repo_) != GIT_OK) {
        return libgitError();
    }
    
    git_revwalk_sorting(walker.get(), GIT_SORT_TIME);
    
    git_oid oid_head;
    if (git_reference_name_to_id(&oid_head, repo_, HEAD) != GIT_OK) {
        return libgitError();
    }
    
    if (git_revwalk_push(walker.get(), &oid_head) != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

FileStatus::FileStatus() : newToHead(false), modFileToHead(false),
                           deleteToHead(false), renameToHead(false),
                           newToDir(false), modFileToDir(false),
                           deleteToDir(false), renameToDir(false) {}

CommitInfo::CommitInfo(const QString& authorName_, const QString& authorEmail_,
                       const QString& commitMessage_, const QString& commitHash_,
                       const QDateTime& commitCreateTime_) :
                       authorName(authorName_), authorEmail(authorEmail_),
                       commitMessage(commitMessage_), commitHash(commitHash_),
                       commitCreateTime(commitCreateTime_) {}


RepoConfigBuf::RepoConfigBuf(const RepoConfig& cfg) :
                             url(cfg.url.toUtf8()), branch(cfg.branch.toUtf8()),
                             path(cfg.path.toUtf8()), username(cfg.username.toUtf8()),
                             token(cfg.token.toUtf8()) {}


IRepository* createRepository(const RepoConfig& cfg){
    git_libgit2_init();
    Repository* repo = new Repository(cfg);
    qRegisterMetaType<size_t>("size_t");

    return repo;
}

void deleteRepository(IRepository *repo){
    delete repo;
    git_libgit2_shutdown();
}


GitError libgitError(){
    const git_error* err = git_error_last();
    if(err && err -> message){
        QString error = QString::fromUtf8(err -> message);

        return GitError(error, err->klass);
    }

    return GitError("unknow", UNKNOW);
}
