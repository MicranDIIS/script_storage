#include "repository.h"

struct GitData{
    QByteArray username;
    QByteArray token;

    GitData(const QString &username_,const QString &token_) : username(username_.toUtf8()) , token(token_.toUtf8()) {}
};

static int callback(git_credential **out,const char *url,
                         const char *username_from_url,
                         unsigned int allowed_types,
                         void *payload)
{
    GitData* data = static_cast<GitData*>(payload);

    if(!data){
        return -1;
    }

    if(!(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)){
        return -1;
    }
    return git_credential_userpass_plaintext_new(out,
                                                 data -> username.constData(),
                                                 data -> token.constData());
}


GitError Repository::clone(){

    git_repository* repo = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    QByteArray branch = cfg_.branch.toUtf8();
    clone_opts.checkout_branch = branch.constData();
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    clone_opts.checkout_opts = opts;

    QByteArray url = cfg_.url.toUtf8();
    QByteArray path = cfg_.path.toUtf8();

    QByteArray username = cfg_.username.toUtf8();
    QByteArray token = cfg_.token.toUtf8();
    GitData creds(username, token);

    clone_opts.fetch_opts.callbacks.credentials = callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo, url.constData(), path.constData(), &clone_opts) != GIT_OK){
        return libgitError();
    }

    git_repository_free(repo);
    return GitError();
}

GitError Repository::fetch(){
    if(repo_ == NULL){
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    git_remote* remote = NULL;
    git_fetch_options fetchopt = GIT_FETCH_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    QByteArray username = cfg_.username.toUtf8();
    QByteArray token = cfg_.token.toUtf8();
    GitData creds(username, token);

    callbacks.credentials = callback;
    callbacks.payload = &creds;
    fetchopt.callbacks = callbacks;
    fetchopt.update_fetchhead = 1;

    if(git_remote_lookup(&remote, repo_, "origin") != GIT_OK){
        return libgitError();
    }

    QString refspec = QString("+refs/heads/%1:refs/remotes/origin/%1")
                      .arg(cfg_.branch);

    QByteArray refspecs_ = refspec.toUtf8();
    char* refs[] = { refspecs_.data() };

    git_strarray refspec_array;
    refspec_array.count = 1;
    refspec_array.strings = refs;

    if(git_remote_fetch(remote, &refspec_array, &fetchopt, NULL) != GIT_OK){
        git_remote_free(remote);
        return libgitError();
    }

    git_remote_free(remote);
    return GitError();
}

GitError Repository::sync(){
    GitError err = fetch();
    if(!err.success){
        return err;
    }

    git_object* obj = NULL;
    QString branch = QString("refs/remotes/origin/%1").arg(cfg_.branch);
    QByteArray branch_ = branch.toUtf8();

    if(git_revparse_single(&obj, repo_, branch_.constData()) != GIT_OK){
        return libgitError();
    }

    if(git_reset(repo_, obj, GIT_RESET_HARD, NULL) != GIT_OK){
        git_object_free(obj);
        return libgitError();
    }

    git_object_free(obj);
    return GitError();
}

void Repository::startCheckUpdatesActiveFile(const FileEventHandler& fileHandler, size_t timeSec){
    handler_ = new UpdateHandler(repo_, fileHandler, cfg_.url,
                                 cfg_.token, cfg_.username, cfg_.branch);
    connect(&timer_, SIGNAL(timeout()), handler_, SLOT(checkUpdatesActiveFile()));
    connect(handler_, SIGNAL(updateIsFound()), this, SLOT(fetch()));

    timer_.start(1000 * timeSec);
}


void Repository::stopCheckUpdatesActiveFile(){
    timer_.stop();
    delete handler_;
}

struct LogFile {
    QString file_path;
    bool found;
};

static int diff_file_callback(const git_diff_delta* delta, float progress,
                              void* payload)
{
    (void)progress;
    LogFile* file = static_cast<LogFile*>(payload);

    const QString old_path = delta->old_file.path ? QString::fromUtf8(delta->old_file.path) : QString();
    const QString new_path = delta->new_file.path ? QString::fromUtf8(delta->new_file.path) : QString();


    if (old_path == file->file_path || new_path == file->file_path) {
        file->found = true;
        return 1;
    }

    return 0;
}


void UpdateHandler::checkUpdatesActiveFile(){
    git_remote* remote = NULL;
    QByteArray url = url_.toUtf8();

    if (git_remote_create_anonymous(&remote, repo_, url.constData()) != GIT_OK) {
        git_remote_free(remote);
        return;
    }

    git_direction direction = GIT_DIRECTION_FETCH;
    QByteArray username = username_.toUtf8();
    QByteArray token = token_.toUtf8();
    GitData creds(username, token);
    git_remote_callbacks callback_ = GIT_REMOTE_CALLBACKS_INIT;
    callback_.credentials = callback;
    callback_.payload = &creds;

    if (git_remote_connect(remote, direction, &callback_, NULL, NULL) != GIT_OK) {
        git_remote_free(remote);
        return;
    }

    const git_remote_head **heads;
    size_t count = 0;
    if (git_remote_ls(&heads, &count, remote) != GIT_OK) {
        git_remote_disconnect(remote);
        git_remote_free(remote);
        return;
    }

    QString branch_local = QString("refs/heads/%1").arg(branch_);
    QString branch_remote_str = "";
    bool branch_found = false;
    git_oid oid_remote;
    memset(&oid_remote, 0, sizeof(oid_remote));

    for (size_t i = 0; i < count; i++) {
        branch_remote_str = QString::fromUtf8(heads[i]->name);
        if (branch_local == branch_remote_str) {
            branch_found = true;
            git_oid_cpy(&oid_remote, &heads[i]->oid);
            break;
        }
    }

    git_remote_disconnect(remote);
    git_remote_free(remote);

    if (!branch_found) {
        return;
    }

    git_oid oid_local;
    if (git_reference_name_to_id(&oid_local, repo_, "HEAD") != GIT_OK) {
        return;
    }

    if (git_oid_cmp(&oid_local, &oid_remote) == 0) {
        return;
    }

    emit updateIsFound();
    QByteArray remoteBranch = QString("refs/remotes/origin/%1").arg(branch_).toUtf8();
    if (git_reference_name_to_id(&oid_remote, repo_, remoteBranch.constData()) != GIT_OK) {
        return;
    }

    git_commit* commit_local = NULL;
    git_commit* commit_remote = NULL;

    if (git_commit_lookup(&commit_local, repo_, &oid_local) != GIT_OK) {
        git_commit_free(commit_local);
        return;
    }

    if (git_commit_lookup(&commit_remote, repo_, &oid_remote) != GIT_OK) {
        git_commit_free(commit_local);
        git_commit_free(commit_remote);
        return;
    }

    git_tree* tree_local = NULL;
    git_tree* tree_remote = NULL;

    if (git_commit_tree(&tree_local, commit_local) != GIT_OK) {
        git_commit_free(commit_local);
        git_commit_free(commit_remote);
        return;
    }

    if (git_commit_tree(&tree_remote, commit_remote) != GIT_OK) {
        git_commit_free(commit_local);
        git_commit_free(commit_remote);
        git_tree_free(tree_local);
        return;
    }

    QByteArray filePath_ = Filehandler_.filePath.toUtf8();
    char* path = filePath_.data();
    char* pathspec[1] = { path };

    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.pathspec.strings = pathspec;
    opts.pathspec.count = 1;

    git_diff* diff = NULL;
    if (git_diff_tree_to_tree(&diff, repo_, tree_local, tree_remote, &opts) != GIT_OK) {
        git_tree_free(tree_local);
        git_tree_free(tree_remote);
        git_commit_free(commit_local);
        git_commit_free(commit_remote);
        return;
    }

    LogFile file = {Filehandler_.filePath, false};

    if (git_diff_foreach(diff, diff_file_callback, NULL, NULL, NULL, &file) < 0) {
        git_diff_free(diff);
        git_tree_free(tree_local);
        git_tree_free(tree_remote);
        git_commit_free(commit_local);
        git_commit_free(commit_remote);
        return;
    }


    git_diff_free(diff);
    git_tree_free(tree_local);
    git_tree_free(tree_remote);
    git_commit_free(commit_local);
    git_commit_free(commit_remote);

    if(file.found){
        Filehandler_.callback();
    }

}


