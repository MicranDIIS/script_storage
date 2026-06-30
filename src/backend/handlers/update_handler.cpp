#include "update_handler.h"

#include "git_raii.h"

UpdateHandler::UpdateHandler(git_repository* repo, const FileEventHandler& Filehandler,
                             const QString& url, const QString& token,
                             const QString& username, const QString& branch) :
    repo_(repo), Filehandler_(Filehandler), url_(url), token_(token),
    username_(username), branch_(branch) {}

void UpdateHandler::checkUpdatesActiveFile(){
    QByteArray url = url_.toUtf8();

    GitRemotePtr remote;
    if (git_remote_create_anonymous(&remote, repo_, url.constData()) != GIT_OK) {
        return;
    }

    git_direction direction = GIT_DIRECTION_FETCH;
    QByteArray username = username_.toUtf8();
    QByteArray token = token_.toUtf8();
    GitData creds(username, token);
    git_remote_callbacks callback_ = GIT_REMOTE_CALLBACKS_INIT;
    callback_.credentials = callback;
    callback_.payload = &creds;

    if (git_remote_connect(remote.get(), direction, &callback_, NULL, NULL) != GIT_OK) {
        return;
    }

    const git_remote_head **heads;
    size_t count = 0;
    if (git_remote_ls(&heads, &count, remote.get()) != GIT_OK) {
        git_remote_disconnect(remote.get());
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

    git_remote_disconnect(remote.get());

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

    GitCommitPtr commit_local;
    GitCommitPtr commit_remote;

    if (git_commit_lookup(&commit_local, repo_, &oid_local) != GIT_OK) {
        return;
    }

    if (git_commit_lookup(&commit_remote, repo_, &oid_remote) != GIT_OK) {
        return;
    }

    GitTreePtr tree_local;
    GitTreePtr tree_remote;

    if (git_commit_tree(&tree_local, commit_local.get()) != GIT_OK) {
        return;
    }

    if (git_commit_tree(&tree_remote, commit_remote.get()) != GIT_OK) {
        return;
    }

    QByteArray filePath_ = Filehandler_.filePath.toUtf8();
    char* path = filePath_.data();
    char* pathspec[1] = { path };

    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.pathspec.strings = pathspec;
    opts.pathspec.count = 1;

    GitDiffPtr diff;
    if (git_diff_tree_to_tree(&diff, repo_, tree_local.get(), tree_remote.get(), &opts) != GIT_OK) {
        return;
    }

    LogFile file = {Filehandler_.filePath, false};

    if (git_diff_foreach(diff.get(), diff_file_callback, NULL, NULL, NULL, &file) < 0) {
        return;
    }

    if (file.found) {
        Filehandler_.callback();
    }
}
