#include "network.h"

//структура для коллбэк ф-ии
struct GitData{
    QByteArray username;
    QByteArray token;

    GitData(const QString &username_,const QString &token_) : username(username_.toUtf8()) , token(token_.toUtf8()) {}
};

//для git clone,git fetch и других
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


//реализация git clone
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
