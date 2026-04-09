#include "../core/git/git_base.hpp"

static QString get_git_error(){
    const git_error *err = git_error_last();
    QString msg = QString(err -> message);
    return msg;
}

//-----------------------------------------------------------------------------
//конструктор и деконструктор
static git_repository *repo_git_open(const QString &path){

    git_repository *repo = nullptr;

    QByteArray path_ = path.toUtf8();
    if(git_repository_open(&repo,path_.constData()) != 0){
        return nullptr;
    }

    return repo;
}

Repo_git::Repo_git(const QString &path_){
    this -> path = path_;
    this -> repo = repo_git_open(path_);
    if(this -> repo == nullptr){
        error = "Ошибка открытия каталога";
    }
    this -> token = "";
}
Repo_git::~Repo_git(){
    if(repo != nullptr){
        git_repository_free(repo);
    }
}
//-----------------------------------------------------------------------------

//геттеры
git_repository *Repo_git::get_repo(){return repo;}
QString Repo_git::get_path(){return path;}
QString Repo_git::get_error(){return error;}

//основное

Repo_git core_git_init(const QString &path){
    return Repo_git(path);
}

//реализация status()
//-----------------------------------------------------------------------------
enum STATUS_FLAG{
    STATUS_NEW_TO_HEAD     = 1 << 0, //новый 
    STATUS_MODFILE_TO_HEAD = 1 << 1, //изменен
    STATUS_DELETE_TO_HEAD  = 1 << 2, //удален
    STATUS_RENAME_TO_HEAD  = 1 << 3, //переименован
    STATUS_MODFILE_TO_DIR  = 1 << 4, //изменен в директории
    STATUS_DELETE_TO_DIR   = 1 << 5, //удален в директории
    STATUS_RENAME_TO_DIR   = 1 << 6, //переименован в директории
    STATUS_NEW_TO_DIR      = 1 << 7 //новый в директории
};


QList<FileStatus> Repo_git::status(){
    QList<FileStatus> list;
    if(repo == nullptr){
        error = "пустой указатель на репозиторий";
        return list;
    }

    git_status_list *status = nullptr;
    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    if(git_status_list_new(&status,repo,&statopt) != 0){
        error = get_git_error();
        return list;
    }

    size_t len = git_status_list_entrycount(status);

    list.reserve(len);
    for(size_t i = 0; i <  len;i++){
        QString file_path_new;
        QString file_path_old;
        int file_status = 0;
        const git_status_entry *entry = git_status_byindex(status,i);
        if(entry == nullptr || entry -> status & GIT_STATUS_CURRENT){
            continue;
        }
        if(entry -> head_to_index && entry -> head_to_index -> new_file.mode & GIT_FILEMODE_TREE){
            continue;
        }
        if(entry -> index_to_workdir && entry -> index_to_workdir -> new_file.mode & GIT_FILEMODE_TREE){
            continue;
        }

        //в HEAD
        if(entry -> head_to_index){
            if(entry -> status & GIT_STATUS_INDEX_NEW){
                file_path_new = QString(entry -> head_to_index -> new_file.path);
                file_path_old = "";
                file_status |= STATUS_NEW_TO_HEAD;
            }
            if(entry -> status & GIT_STATUS_INDEX_RENAMED){
                file_path_new = QString(entry -> head_to_index -> new_file.path);
                file_path_old = QString(entry -> head_to_index -> old_file.path);
                file_status |= STATUS_RENAME_TO_HEAD;
            }
            if(entry -> status & GIT_STATUS_INDEX_DELETED){
                file_path_new = "";
                file_path_old = QString(entry -> head_to_index -> old_file.path);
                file_status |= STATUS_DELETE_TO_HEAD;
            }
            if(entry -> status & GIT_STATUS_INDEX_MODIFIED){
                file_path_new = QString(entry -> head_to_index -> new_file.path);
                file_path_old = file_path_new;
                file_status |= STATUS_MODFILE_TO_HEAD;
            }

        }

        if(entry -> index_to_workdir){
            if(entry -> status & GIT_STATUS_WT_NEW){
                file_path_new = QString(entry -> index_to_workdir -> new_file.path);
                file_path_old = "";
                file_status |= STATUS_NEW_TO_DIR;
            }
            if(entry -> status & GIT_STATUS_WT_RENAMED){
                file_path_new = QString(entry -> index_to_workdir -> new_file.path);
                file_path_old = QString(entry -> index_to_workdir -> old_file.path);
                file_status |= STATUS_RENAME_TO_DIR;
            }
            if(entry -> status & GIT_STATUS_WT_DELETED){
                file_path_new = "";
                file_path_old = QString(entry -> index_to_workdir -> old_file.path);
                file_status |= STATUS_DELETE_TO_DIR;
            }
            if(entry -> status & GIT_STATUS_WT_MODIFIED){
                file_path_new = QString(entry -> index_to_workdir -> new_file.path);
                file_path_old = file_path_new;
                file_status |= STATUS_MODFILE_TO_DIR;
            }
        }

        if(file_status != 0){
            list.append(FileStatus(file_path_new,file_path_old,file_status));
        }
    }

    git_status_list_free(status);

    return list;
}
//-----------------------------------------------------------------------------

//Реализация git clone
//-----------------------------------------------------------------------------
struct GitCreds{
    QByteArray username;
    QByteArray token;

    GitCreds(const QString &username_,const QString &token_) : username(username_.toUtf8()) , token(token_.toUtf8()) {}
};

//сама коллбэк ф-я 
static int cred_callback(git_credential **out,const char *url,
                         const char *username_from_url,
                         unsigned int allowed_types,
                         void *payload)
{
    GitCreds *creds = static_cast<GitCreds*>(payload);

    if(!creds || creds -> username.isEmpty() || creds -> token.isEmpty()){
        return -1;
    }

    if(!(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)){
        return -1;
    }
    return git_credential_userpass_plaintext_new(out,
                                                 creds -> username.constData(),
                                                 creds -> token.constData());
}

//git clone
int core_git_clone(const QString &URL,const QString &path,const QString &username,const QString &token)
{
    if(URL.isEmpty() || path.isEmpty() || token.isEmpty() || username.isEmpty()){
        return -1;
    }
    git_repository *repo = nullptr;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    

    QByteArray url = URL.toUtf8();
    QByteArray path_ = path.toUtf8();

    GitCreds creds(username,token);

    clone_opts.fetch_opts.callbacks.credentials = cred_callback;
    clone_opts.fetch_opts.callbacks.payload = &creds;

    if(git_clone(&repo,url.constData(),path_.constData(),&clone_opts) != 0){
        return -1;
    }

    git_repository_free(repo);

    return 0;
}

//Реализация git pull
//-----------------------------------------------------------------------------
/*int Repo_git::pull(const QString &username, const QString &token)
{
    if(repo == nullptr){
        error = "пустой указатель на репозиторий";
        return -1;
    }
    
    if(token.isEmpty() || username.isEmpty()){
        error = "токен или имя пользователя не может быть пустым";
        return -1;
    }
    
    git_remote *remote = nullptr;
    
    // Получаем remote "origin"
    if(git_remote_lookup(&remote, repo, "origin") != 0){
        error = get_git_error();
        return -1;
    }
    
    // Настройка опций для fetch
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    
    // Настройка аутентификации
    GitCreds creds(username, token);
    fetch_opts.callbacks.credentials = cred_callback;
    fetch_opts.callbacks.payload = &creds;
    
    // Выполняем fetch
    if(git_remote_fetch(remote, nullptr, &fetch_opts, nullptr) != 0){
        error = get_git_error();
        git_remote_free(remote);
        return -1;
    }
    
    // Получаем ссылку на текущую ветку
    git_reference *head_ref = nullptr;
    if(git_repository_head(&head_ref, repo) != 0){
        error = get_git_error();
        git_remote_free(remote);
        return -1;
    }
    
    const char *branch_name = git_reference_shorthand(head_ref);
    if(branch_name == nullptr){
        error = "не удалось получить имя текущей ветки";
        git_reference_free(head_ref);
        git_remote_free(remote);
        return -1;
    }
    
    git_annotated_commit *upstream_commit = nullptr;
    QString upstream_branch = QString("origin/%1").arg(branch_name);
    QByteArray upstream_bytes = upstream_branch.toUtf8();
    
    if(git_annotated_commit_from_revspec(&upstream_commit, repo, upstream_bytes.constData()) != 0){
        error = get_git_error();
        git_reference_free(head_ref);
        git_remote_free(remote);
        return -1;
    }
    
    // Выполняем слияние
    git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    int result = git_merge(repo, &upstream_commit, 1, &merge_opts, &checkout_opts);
    
    if(result != 0){
        error = get_git_error();
    }
    
    git_annotated_commit_free(upstream_commit);
    git_reference_free(head_ref);
    git_remote_free(remote);
    
    return result == 0 ? 0 : -1;
}*/
//-----------------------------------------------------------------------------