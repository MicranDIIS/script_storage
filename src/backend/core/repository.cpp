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
QString Repository::getDebugFilePath() const {return debugCtx.debugFilePath;}
bool Repository::isValid() const {return repo_ != NULL;}

const char* Repository::HEAD = "HEAD";
const char* Repository::ORIGIN = "origin";

GitError Repository::getTimeLastRemoteCommit(QTime &time) const{
    GitCommitPtr commit;
    if(git_revparse_single((git_object**)&commit, repo_,
                           (QByteArray("refs/remotes/origin/") + cfg_.branch).constData()) != GIT_OK){
        return libgitError();
    }

    const git_signature* author = git_commit_author(commit.get());

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(1000 * author->when.time);
    time = dateTime.time();

    return GitError();
}

static int check_file_in_head(const GitObjectPtr& obj, const char* filePath){
    GitTreePtr tree;
    if(git_commit_tree(&tree, (git_commit*)obj.get()) != GIT_OK){
        return -1;
    }

    git_tree_entry* entry = NULL;
    if(git_tree_entry_bypath(&entry, tree.get(), filePath) != GIT_OK){
        return -1;
    }

    if(entry == NULL){
        return -1;
    }

    return 0;
}

GitError Repository::setDebugFilePath(const QString &filePath){
    if(!debugCtx.debugFilePath.isEmpty()){
        return GitError("debugFile is not empty", -1);
    }

    QByteArray filePath_ = filePath.toUtf8();
    GitObjectPtr obj;
    if(git_revparse_single(&obj, repo_, "HEAD") != GIT_OK){
        return libgitError();
    }

    if(git_object_type(obj.get()) != GIT_OBJECT_COMMIT){
        return GitError("HEAD is not a commit", -1);
    }

    if(check_file_in_head(obj, filePath_.constData()) != 0){
        return GitError("file not found in HEAD", -1);
    }

    const git_oid* oid = git_commit_id((git_commit*)obj.get());
    if(!oid){
        return GitError("failed to get commit id", -1);
    }

    debugCtx.debugFilePath = filePath_;
    debugCtx.parent_oid = *oid;

    return GitError();
}

GitError Repository::swapDebugFilePath(const QString &newFilePath){
    if(debugCtx.debugFilePath.isEmpty()){
        return GitError("debugFilePath is empty", -1);
    }

    if(git_oid_iszero(&debugCtx.parent_oid)){
        return GitError("parent commit not set", -1);
    }

    QByteArray newFilePath_ = newFilePath.toUtf8();

    GitObjectPtr headObj;
    if(git_revparse_single(&headObj, repo_, "HEAD") != GIT_OK){
        return libgitError();
    }

    if(check_file_in_head(headObj, newFilePath_.constData()) != 0){
        return GitError("file not found in HEAD", -1);
    }

    GitObjectPtr parentCommit;
    if(git_object_lookup(&parentCommit, repo_, &debugCtx.parent_oid, GIT_OBJECT_COMMIT) != GIT_OK){
        return libgitError();
    }

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_FORCE;

    if(git_reset(repo_, parentCommit.get(), GIT_RESET_HARD, &opts) != GIT_OK){
        return libgitError();
    }

    debugCtx.debugFilePath = newFilePath_;

    return GitError();
}

bool Repository::isValidRepo() const {
    if (repo_ == NULL) {
        return false;
    }

    git_reference *head_ref_raw = NULL;
    if (git_repository_head(&head_ref_raw, repo_) != GIT_OK) {
        return false;
    }
    GitReferencePtr head_ref(head_ref_raw);

    const git_oid *head_oid = git_reference_target(head_ref.get());
    if (head_oid == NULL) {
        return false;
    }

    GitCommitPtr head_commit;
    if (git_commit_lookup(&head_commit, repo_, head_oid) != GIT_OK) {
        return false;
    }

    GitTreePtr head_tree;
    if (git_commit_tree(&head_tree, head_commit.get()) != GIT_OK) {
        return false;
    }

    GitIndexPtr index;
    if (git_repository_index(&index, repo_) != GIT_OK) {
        return false;
    }

    size_t entry_count = git_index_entrycount(index.get());

    if (entry_count > 0) {
        GitOdbPtr odb;
        if (git_repository_odb(&odb, repo_) != GIT_OK) {
            return false;
        }

        for (size_t i = 0; i < entry_count; i++) {
            const git_index_entry *entry = git_index_get_byindex(index.get(), i);
            if (entry == NULL) {
                continue;
            }

            GitOdbObjectPtr obj;
            if (git_odb_read(&obj, odb.get(), &entry->id) != GIT_OK) {
                return false;
            }
        }
    }

    GitBranchIteratorPtr iter;
    bool has_branch = false;
    if (git_branch_iterator_new(&iter, repo_, GIT_BRANCH_LOCAL) == GIT_OK) {
        git_reference *branch_ref_raw = NULL;
        git_branch_t branch_type;
        if (git_branch_next(&branch_ref_raw, &branch_type, iter.get()) == GIT_OK) {
            has_branch = true;
            git_reference_free(branch_ref_raw);
        }
    }

    if (!has_branch) {
        if (git_repository_head_detached(repo_) != 1) {
            return false;
        }
    }

    GitConfigPtr config;
    if (git_repository_config(&config, repo_) != GIT_OK) {
        return false;
    }

    return true;
}

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

authRemoteBuf::authRemoteBuf(const authRemote& auth) :
                             username(auth.username.toUtf8()),
                             token(auth.token.toUtf8()){}

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
