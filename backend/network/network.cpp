#include "network.h"

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



typedef GitData GitClone;

Repo Repo::clone(const QString &URL,const QString &path,const QString &username,const QString &token){
    if(URL.isEmpty() || path.isEmpty() || token.isEmpty() || username.isEmpty()){
        return Repo(path,token);
    }
    git_repository *repo = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;


    QByteArray url = URL.toUtf8();
    QByteArray path_ = path.toUtf8();

    GitClone creds(username,token);

    clone_opts.fetch_opts.callbacks.credentials = callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo,url.constData(),path_.constData(),&clone_opts) != 0){
        Repo repo_err(path,token);

        return repo_err;
    }

    git_repository_free(repo);

    return Repo(path,token);
}

typedef GitData GitFetch;

#include <string.h>
int Repo::fetch(void){
    git_remote *remote = NULL;
    git_fetch_options fetchopt = GIT_FETCH_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    callbacks.credentials = callback;
    callbacks.payload = this;
    fetchopt.callbacks = callbacks;
    fetchopt.update_fetchhead = 1;

    git_reference *head = NULL;
    git_buf remote_buf = GIT_BUF_INIT;
    const char *remote_name = NULL;

    if(git_repository_head(&head,repo) != 0){
        return -1;
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
        git_reference_free(head);
        git_buf_free(&remote_buf);
        return -1;
    }

    if(git_remote_lookup(&remote,repo,remote_name) != 0){
        git_reference_free(head);
        git_buf_free(&remote_buf);
        return -1;
    }

    if(git_remote_fetch(remote,NULL,&fetchopt,NULL) != 0){
        git_remote_free(remote);
        git_reference_free(head);
        git_buf_free(&remote_buf);
        return -1;
    }

    git_remote_free(remote);
    git_reference_free(head);
    git_buf_free(&remote_buf);

    return 0;
}
