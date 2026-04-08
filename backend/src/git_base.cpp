#include "../core/git/git_base.hpp"

//это по задумке основная структура которая в дальнейшем будем расширяться и тд. Мб лучше юзать классы, но я не уверен
struct git_t{
    git_repository *repo;
    QString path;

    //конструктор и деконструктор
    git_t(git_repository *r,const QString &p) : repo(r) , path(p) {}
    ~git_t(){if(repo) git_repository_free(repo);}
};

git_repository *git_get_repo(git_t *gt){
    if(gt == nullptr){
        return nullptr;
    }
    return gt->repo;
}

//все что ниже это реализация git clone https://...
//-----------------------------------------------------------------------------
//для коллбэк ф-ии
struct GitCreds{
    QByteArray token;
};

//определяет по токену к ГитЛабу мы обращаемся или к Гитхабу. Вообще смотря на URL можно, но я не знаю. Просто почему-то сделал так
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

//сама коллбэк ф-я 
static int cred_callback(git_credential **out,const char *url,
                          const char *username_from_url,unsigned int allowed_types,
                           void *payload)
{
    GitCreds *creds = static_cast<GitCreds*>(payload);

    QByteArray is_repo = token_determinant(creds -> token);

    return git_credential_userpass_plaintext_new(out,is_repo.constData(),creds -> token.constData());
}

//git clone
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
//конец реализации git clone


// Простая версия pull - только fetch и merge с origin/current_branch
int core_git_pull(git_t *gt, const QString &token)
{
    if(gt == nullptr || gt->repo == nullptr){
        return -1;
    }
    
    git_remote *remote = nullptr;
    git_repository *repo = gt->repo;
    
    // Получаем remote "origin"
    if(git_remote_lookup(&remote, repo, "origin") != 0){
        return -1;
    }
    
    // Настройка опций для fetch
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    
    // Если передан токен, настраиваем авторизацию
    if(!token.isEmpty()){
        GitCreds creds;
        creds.token = token.toUtf8();
        fetch_opts.callbacks.credentials = cred_callback;
    }
    
    // Выполняем fetch
    if(git_remote_fetch(remote, nullptr, &fetch_opts, nullptr) != 0){
        git_remote_free(remote);
        return -1;
    }
    
    // Получаем ссылку на текущую ветку
    git_reference *head_ref = nullptr;
    if(git_repository_head(&head_ref, repo) != 0){
        git_remote_free(remote);
        return -1;
    }
    
    const char *branch_name = git_reference_shorthand(head_ref);
    
    // Формируем имя upstream ветки
    QString upstream_branch = QString("refs/remotes/origin/%1").arg(branch_name);
    QByteArray upstream_bytes = upstream_branch.toUtf8();
    
    // Получаем annotated commit для upstream ветки
    git_annotated_commit *upstream_commit = nullptr;
    if(git_annotated_commit_lookup(&upstream_commit, repo, upstream_bytes.constData()) != 0){
        git_reference_free(head_ref);
        git_remote_free(remote);
        return -1;
    }
    
    // Выполняем слияние
    git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    int result = git_merge(repo, &upstream_commit, 1, &merge_opts, &checkout_opts);
    
    // Очистка
    git_annotated_commit_free(upstream_commit);
    git_reference_free(head_ref);
    git_remote_free(remote);
    
    return result == 0 ? 0 : -1;
}

//открывает локальный репозиторий для работы
git_t *core_git_open(const QString &path)
{
    git_repository *repo = nullptr;
    QByteArray path_ = path.toUtf8();

    if(git_repository_open(&repo,path_.constData()) != 0){
        return nullptr;
    }

    return new git_t(repo,path);
}

QList<QString> core_git_status(git_repository *repo)
{
    QList<QString> list;
    if(repo == nullptr){
        return list;
    }

    git_status_list *status = nullptr;
    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;
    
    if(git_status_list_new(&status,repo,&statopt) != 0){
        return list;
    }

    size_t len = git_status_list_entrycount(status);

    for(size_t i = 0;i < len;i++){
        QString file_path;
        QString file_status_type = "";
        const git_status_entry *entry = git_status_byindex(status,i);
        if(entry == nullptr || entry ->status & GIT_STATUS_CURRENT){
            continue;
        }
        if(entry -> head_to_index && entry -> head_to_index -> new_file.mode == GIT_FILEMODE_TREE){
            continue;
        }

        if(entry -> index_to_workdir && entry -> index_to_workdir -> new_file.mode == GIT_FILEMODE_TREE){
            continue;
        }
        if(entry -> status & GIT_STATUS_INDEX_NEW){
            if(entry ->head_to_index){
                file_path = QString(entry -> head_to_index -> new_file.path);
                file_status_type += "Новый в HEAD ";
            }
        }
        if(entry -> status & GIT_STATUS_INDEX_RENAMED){
            if(entry -> head_to_index){
                file_path = QString(entry -> head_to_index -> old_file.path);
                file_status_type += "Переименован в HEAD ";
            }
        }
        if(entry -> status & GIT_STATUS_INDEX_MODIFIED){
            if(entry -> head_to_index){
                file_path = QString(entry -> head_to_index -> new_file.path);
                file_status_type += "Изменен в HEAD ";
            }
        }
        if(entry -> status & GIT_STATUS_INDEX_DELETED){
            if(entry -> head_to_index){
                file_path = QString(entry -> head_to_index -> old_file.path);
                file_status_type += "Удален в HEAD ";
            }
        }
        if(entry -> status & GIT_STATUS_WT_NEW){
            if(entry -> index_to_workdir){
                file_path = QString(entry -> index_to_workdir -> new_file.path);
                file_status_type += "Новый в директории ";
            }
        }
        if(entry -> status & GIT_STATUS_WT_MODIFIED){
            if(entry -> index_to_workdir){
                file_path = QString(entry -> index_to_workdir -> new_file.path);
                file_status_type += "Изменен в директории ";
            }
        }
        if(entry -> status & GIT_STATUS_WT_DELETED){
            if(entry -> index_to_workdir){
                file_path = QString(entry -> index_to_workdir -> old_file.path);
                file_status_type += "Удален в директории ";
            }
        }
        if(!file_path.isEmpty()){
            list.append(file_path + " " + file_status_type);
        }
    
    }

    git_status_list_free(status);

    return list;
}