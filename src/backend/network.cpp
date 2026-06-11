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
    if(err.isError()){
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
