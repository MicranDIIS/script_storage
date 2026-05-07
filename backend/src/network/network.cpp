#include "../Repository/repository.h"

static QString git_get_error() { 
    const git_error* err = git_error_last();
    if (err && err->message) {
        return QString::fromUtf8(err->message);
    }
    return "Unknown";
}

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
    GitData *data = static_cast<GitData*>(payload);

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

static int repo_has_remote_branch(git_repository* repo, const QString& branch){
    git_reference* ref = NULL;
    QByteArray branch_ = QString("refs/remotes/origin/%1").arg(branch).toUtf8();

    if(git_reference_lookup(&ref, repo, branch_.constData()) != 0){
        return -1;
    }

    git_reference_free(ref);
    return 0;
}

Gerror Repository::clone(){
    if(repo != NULL){
        QString error = "repo is not NULL";
        return Gerror(error);
    }

    git_repository *repo_ = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

    QByteArray url = cfg.url.toUtf8();
    QByteArray path_ = cfg.path.toUtf8();

    QByteArray username_ = cfg.username.toUtf8();
    QByteArray token_ = cfg.token.toUtf8();
    GitData creds(username_,token_);

    clone_opts.fetch_opts.callbacks.credentials = callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo_, url.constData(), path_.constData(), &clone_opts) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    repo = repo_;
    if(cfg.branch == "main" || cfg.branch == "master"){
        return Gerror();
    }

    if(repo_has_remote_branch(repo, cfg.branch) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    if(repo_remote_branch_create_to_local(repo, cfg.branch) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    if(repo_checkout_local_branch(repo, cfg.branch) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }
    return Gerror();
}

Gerror Repository::fetch(){
    if(repo == NULL){
        return Gerror("repo is NULL");
    }
    
    git_remote *remote = NULL;
    git_fetch_options fetchopt = GIT_FETCH_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
    
    QByteArray username_ = cfg.username.toUtf8();
    QByteArray token_ = cfg.token.toUtf8();
    GitData creds(username_, token_);
    
    callbacks.credentials = callback;
    callbacks.payload = &creds;
    fetchopt.callbacks = callbacks;
    fetchopt.update_fetchhead = 1;
    
    if(git_remote_lookup(&remote, repo, "origin") != 0){
        QString error = git_get_error();
        return Gerror(error);
    }
    
    QString refspec = QString("+refs/heads/%1:refs/remotes/origin/%1")
                      .arg(cfg.branch);
                      
    QByteArray refspecs_ = refspec.toUtf8();
    const char *refspecs[] = { refspecs_.constData() };

    git_strarray refspec_array;
    refspec_array.count = 1;
    refspec_array.strings = (char**)refspecs;
    
    if(git_remote_fetch(remote, &refspec_array, &fetchopt, NULL) != 0){
        QString error = git_get_error();
        git_remote_free(remote);
        return Gerror(error);
    }
    
    git_remote_free(remote);
    return Gerror();
}

Gerror Repository::sync(){
    Gerror err = fetch();
    if(!err.succses){
        return err;
    }

    git_object *obj = NULL;
    QString branch = QString("refs/remotes/origin/%1").arg(cfg.branch);
    QByteArray branch_ = branch.toUtf8();

    if(git_revparse_single(&obj, repo, branch_.constData()) != 0){ 
        QString error = git_get_error();
        return Gerror(error);
    }

    if(git_reset(repo, obj, GIT_RESET_HARD, NULL) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    return Gerror();
}
