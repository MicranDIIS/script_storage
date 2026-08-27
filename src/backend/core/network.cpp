#include "repository.h"

GitError Repository::clone(){
    GitRepositoryPtr repo;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    QByteArray branch = cfg_.branch;
    clone_opts.checkout_branch = branch.constData();
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    clone_opts.checkout_opts = opts;

    GitData creds(cfg_.username, cfg_.token);

    clone_opts.fetch_opts.callbacks.credentials = callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo, cfg_.url.constData(), cfg_.path.constData(), &clone_opts) != GIT_OK){
        return libgitError();
    }

    return GitError();
}

GitError Repository::fetch(){
    if(repo_ == NULL){
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    git_fetch_options fetchopt = GIT_FETCH_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    GitData creds(cfg_.username, cfg_.token);

    callbacks.credentials = callback;
    callbacks.payload = &creds;
    fetchopt.callbacks = callbacks;
    fetchopt.update_fetchhead = 1;

    GitRemotePtr remote;
    if(git_remote_lookup(&remote, repo_, ORIGIN) != GIT_OK){
        return libgitError();
    }

    QString refspec = QString("+refs/heads/%1:refs/remotes/origin/%1")
                      .arg(QString::fromUtf8(cfg_.branch));

    QByteArray refspecs_ = refspec.toUtf8();
    char* refs[] = { refspecs_.data() };

    git_strarray refspec_array;
    refspec_array.count = 1;
    refspec_array.strings = refs;

    if(git_remote_fetch(remote.get(), &refspec_array, &fetchopt, NULL) != GIT_OK){
        return libgitError();
    }

    return GitError();
}

GitError Repository::sync(){
    GitError err = fetch();
    if(!err.success){
        return err;
    }

    QString branch = QString("refs/remotes/origin/%1").arg(QString::fromUtf8(cfg_.branch));
    QByteArray branch_ = branch.toUtf8();

    GitObjectPtr obj;
    if(git_revparse_single(&obj, repo_, branch_.constData()) != GIT_OK){
        return libgitError();
    }

    git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
    opt.checkout_strategy = GIT_CHECKOUT_FORCE |
                                GIT_CHECKOUT_REMOVE_UNTRACKED |
                                GIT_CHECKOUT_REMOVE_IGNORED;

    if(git_checkout_tree(repo_, obj.get(), &opt) != GIT_OK){
        return libgitError();
    }

    if(git_reset(repo_, obj.get(), GIT_RESET_HARD, NULL) != GIT_OK){
        return libgitError();
    }

    return GitError();
}

GitError Repository::push(const QString& login,
                          const QString& password) const{

    QString push_url = cfg_.url;
    push_url.replace(cfg_.username + ":" + cfg_.token, "");
    QByteArray push_url_ = push_url.toUtf8();
    if(git_remote_set_pushurl(repo_, "origin", push_url_.constData()) != GIT_OK){
        return libgitError();
    }

    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    GitRemotePtr remote;
    git_remote_callbacks callbacks;
    QByteArray refs = "refs/heads/" + cfg_.branch;
    const char *refs_array[] = {refs.constData()};
    const git_strarray refspecs = {
        const_cast<char**>(refs_array),
        1
    };

    if(git_remote_lookup(&remote, repo_, "origin") != GIT_OK){
        return libgitError();
    }

    if(git_remote_init_callbacks(&callbacks, GIT_REMOTE_CALLBACKS_VERSION) != GIT_OK){
        return libgitError();
    }

    QByteArray login_ = login.toUtf8();
    QByteArray password_ = password.toUtf8();

    GitData creds(login_.constData(), password_.constData());
    callbacks.payload = &creds;
    callbacks.credentials = callback;

    if(git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION) != GIT_OK){
        return libgitError();
    }
    opts.callbacks = callbacks;

    if(git_remote_push(remote.get(), &refspecs, &opts) != GIT_OK){
        return libgitError();
    }

    return GitError();
}

GitError Repository::startCheckUpdatesActiveFile(const FileEventHandler& fileHandler, size_t timeSec){
    if(handler_ != NULL){
        return GitError("fileHandler is not NULL", FILEHANDLER_IS_NOT_NULL);
    }
    handler_ = new UpdateHandler(repo_, fileHandler, cfg_.url,
                                 cfg_.token, cfg_.username, cfg_.branch);
    connect(&timer_, SIGNAL(timeout()), handler_, SLOT(checkUpdatesActiveFile()));
    connect(handler_, SIGNAL(updateIsFound()), this, SLOT(fetch()));

    timer_.start(1000 * timeSec);
    return GitError();
}

void Repository::stopCheckUpdatesActiveFile(){
    timer_.stop();
    if(handler_ != NULL){
        disconnect(&timer_, SIGNAL(timeout()), handler_, SLOT(checkUpdatesActiveFile()));
        disconnect(handler_, SIGNAL(updateIsFound()), this, SLOT(fetch()));
        delete handler_;
        handler_ = NULL;
    }
}

