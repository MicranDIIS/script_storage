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

    if(!data || data -> username.isEmpty() || data -> token.isEmpty()){
        return -1;
    }

    if(!(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)){
        return -1;
    }
    return git_credential_userpass_plaintext_new(out,
                                                 data -> username.constData(),
                                                 data -> token.constData());
}



Gerror Repository::gitClone(const QString& URL,const QString& username){
    if(token.isEmpty() || username.isEmpty() || URL.isEmpty()){
        QString error = "token or username or URL is empty";
        return Gerror(error);
    }
    if(repo != NULL){
        QString error = "repo is already there";
        return Gerror(error);
    }

    git_repository *repo_ = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

    QByteArray url = URL.toUtf8();
    QByteArray path_ = path.toUtf8();

    GitData creds(username,token);

    clone_opts.fetch_opts.callbacks.credentials = callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo_,url.constData(),path_.constData(),&clone_opts) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    repo = repo_;
    
    return Gerror();
}

#include <string.h>
Gerror Repository::gitFetch(const QString& username){
    if(repo == NULL || username.isEmpty() || token.isEmpty()){
        QString error = "repo is NULL or username is empty or token is empty";
        return Gerror(error);
    }
    git_remote *remote = NULL;
    git_fetch_options fetchopt = GIT_FETCH_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    GitData creds(username,token);

    callbacks.credentials = callback;
    callbacks.payload = &creds;
    fetchopt.callbacks = callbacks;
    fetchopt.update_fetchhead = 1;

    git_reference *head = NULL;
    git_buf remote_buf = GIT_BUF_INIT;
    const char *remote_name = NULL;

    if(git_repository_head(&head,repo) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    const char *branch_name = NULL;
    if(git_branch_name(&branch_name,head) == 0){
        if(git_branch_upstream_remote(&remote_buf,repo,branch_name) == 0){
            remote_name = remote_buf.ptr;
        }else{
            remote_name = "origin";
        }
    }else{
        remote_name = "origin";
    }

    if(!remote_name || strlen(remote_name) == 0){
        QString error = git_get_error();

        git_reference_free(head);
        git_buf_free(&remote_buf);

        return Gerror(error);
    }

    if(git_remote_lookup(&remote,repo,remote_name) != 0){
        QString error = git_get_error();

        git_reference_free(head);
        git_buf_free(&remote_buf);

        return Gerror(error);
    }

    if(git_remote_fetch(remote,NULL,&fetchopt,NULL) != 0){
        QString error = git_get_error();

        git_remote_free(remote);
        git_reference_free(head);
        git_buf_free(&remote_buf);

        return Gerror(error);
    }

    git_remote_free(remote);
    git_reference_free(head);
    git_buf_free(&remote_buf);

    return Gerror();
}

Gerror Repository::gitPull(){

}

