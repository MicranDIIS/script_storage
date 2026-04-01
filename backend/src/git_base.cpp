#include "../core/git/git_base.hpp"


struct git_t{
    git_repository *repo;
    QString path;

    //конструктор и деконструктор
    git_t(git_repository *r,const QString &p) : repo(r) , path(p) {}
    ~git_t(){if(repo) git_repository_free(repo);}
};

//-----------------------------------------------------------------------------
struct GitCreds{
    QByteArray token;
};


static QByteArray token_determinant(const QByteArray& token) {
    if (token.startsWith("ghp_") || 
        token.startsWith("ghs_") || 
        token.startsWith("ghu_") || 
        token.startsWith("gho_") || 
        token.startsWith("ghr_")) {
        return "github.com";
    }
    else if (token.startsWith("glpat-") || 
             token.startsWith("glptt-") || 
             token.startsWith("glft-") || 
             token.startsWith("glso-") || 
             token.startsWith("glrt-")) {
        return "oauth2";
    }
    else {
        return "";
    }
}

static int cred_callback(git_credential **out,const char *url,
                          const char *username_from_url,unsigned int allowed_types,
                           void *payload)
{
    GitCreds *creds = static_cast<GitCreds*>(payload);

    QByteArray is_repo = token_determinant(creds -> token);

    return git_credential_userpass_plaintext_new(out,is_repo.constData(),creds -> token.constData());
}

int core_git_clone(const QString &URL,const QString &path,const QString &token)
{
    if(URL.isEmpty() || path.isEmpty() || token.isEmpty()){
        return -1;
    }
    git_repository *repo = nullptr;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    

    QByteArray url = URL.toUtf8();
    QByteArray path_ = path.toUtf8();

    GitCreds creds;
    creds.token = token.toUtf8();

    clone_opts.fetch_opts.callbacks.credentials = cred_callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo,url.constData(),path_.constData(),&clone_opts) != 0){
        return -1;
    }

    git_repository_free(repo);

    return 0;
}
//-----------------------------------------------------------------------------

git_t *core_git_open(const QString &path)
{
    git_repository *repo = nullptr;
    QByteArray path_ = path.toUtf8();

    if(git_repository_open(&repo,path_.constData()) != 0){
        return nullptr;
    }

    return new git_t(repo,path);
}
