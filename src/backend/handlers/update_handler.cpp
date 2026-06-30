#include "update_handler.h"

UpdateHandler::UpdateHandler(git_repository* repo, const FileEventHandler& Filehandler,
                             const QString& url, const QString& token,
                             const QString& username, const QString& branch) :
    repo_(repo), Filehandler_(Filehandler), url_(url), token_(token),
    username_(username), branch_(branch) {}

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
