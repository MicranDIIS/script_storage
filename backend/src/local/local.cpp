#include "../Repository/repository.h"

static QString git_get_error() { 
    const git_error* err = git_error_last();
    if (err && err->message) {
        return QString::fromUtf8(err->message);
    }
    return "Unknown";
}

static int repo_has_local_branch(git_repository* repo, const QString& branch){
    git_reference* ref = NULL;
    QByteArray branch_ = QString("refs/heads/%1").arg(branch).toUtf8();

    if(git_reference_lookup(&ref, repo, branch_.constData()) != 0){
        return -1;
    }

    git_reference_free(ref);
    return 0;
}

Gerror Repository::open(){
    git_repository *repo_ = NULL;

    QByteArray path_ = cfg.path.toUtf8();
    if(git_repository_open(&repo_,path_.constData()) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    repo = repo_;

    if(repo_has_local_branch(repo, cfg.branch) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }

    if(repo_checkout_local_branch(repo, cfg.branch) != 0){
        QString error = git_get_error();
        return Gerror(error);
    }
    return Gerror();
}

Gerror Repository::status(QList<FileStatus>& list) const {
    if (repo == NULL) {
        return Gerror("repo is NULL");
    }

    git_status_list* status = NULL;
    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    if (git_status_list_new(&status, repo, &statopt) != 0) {
        return Gerror(git_get_error());
    }

    size_t len = git_status_list_entrycount(status);
    list.clear();
    list.reserve(len);
    
    for (size_t i = 0; i < len; i++) {
        QString file_path_new;
        QString file_path_old;
        int file_status = 0;
        const git_status_entry* entry = git_status_byindex(status, i);
        
        if (entry == NULL || (entry->status & GIT_STATUS_CURRENT)) {
            continue;
        }
        if (entry->head_to_index && (entry->head_to_index->new_file.mode & GIT_FILEMODE_TREE)) {
            continue;
        }
        if (entry->index_to_workdir && (entry->index_to_workdir->new_file.mode & GIT_FILEMODE_TREE)) {
            continue;
        }

        // В HEAD (индекс)
        if (entry->head_to_index) {
            if (entry->status & GIT_STATUS_INDEX_NEW) {
                file_path_new = QString::fromUtf8(entry->head_to_index->new_file.path);
                file_path_old = "";
                file_status |= STATUS_NEW_TO_HEAD;
            }
            if (entry->status & GIT_STATUS_INDEX_RENAMED) {
                file_path_new = QString::fromUtf8(entry->head_to_index->new_file.path);
                file_path_old = QString::fromUtf8(entry->head_to_index->old_file.path);
                file_status |= STATUS_RENAME_TO_HEAD;
            }
            if (entry->status & GIT_STATUS_INDEX_DELETED) {
                file_path_new = "";
                file_path_old = QString::fromUtf8(entry->head_to_index->old_file.path);
                file_status |= STATUS_DELETE_TO_HEAD;
            }
            if (entry->status & GIT_STATUS_INDEX_MODIFIED) {
                file_path_new = QString::fromUtf8(entry->head_to_index->new_file.path);
                file_path_old = file_path_new;
                file_status |= STATUS_MODFILE_TO_HEAD;
            }
        }

        // В рабочей директории
        if (entry->index_to_workdir) {
            if (entry->status & GIT_STATUS_WT_NEW) {
                file_path_new = QString::fromUtf8(entry->index_to_workdir->new_file.path);
                file_path_old = "";
                file_status |= STATUS_NEW_TO_DIR;
            }
            if (entry->status & GIT_STATUS_WT_RENAMED) {
                file_path_new = QString::fromUtf8(entry->index_to_workdir->new_file.path);
                file_path_old = QString::fromUtf8(entry->index_to_workdir->old_file.path);
                file_status |= STATUS_RENAME_TO_DIR;
            }
            if (entry->status & GIT_STATUS_WT_DELETED) {
                file_path_new = "";
                file_path_old = QString::fromUtf8(entry->index_to_workdir->old_file.path);
                file_status |= STATUS_DELETE_TO_DIR;
            }
            if (entry->status & GIT_STATUS_WT_MODIFIED) {
                file_path_new = QString::fromUtf8(entry->index_to_workdir->new_file.path);
                file_path_old = file_path_new;
                file_status |= STATUS_MODFILE_TO_DIR;
            }
        }

        if (file_status != 0) {
            list.append(FileStatus(file_path_new, file_path_old, file_status));
        }
    }

    git_status_list_free(status);
    return Gerror();
}

Gerror Repository::reset() {
    if (repo == NULL) {
        return Gerror("repo is NULL");
    }
    
    git_object* obj = NULL;
    
    if (git_revparse_single(&obj, repo, "HEAD") != 0) {
        QString error = git_get_error();
        return Gerror(error);
    }
    
    git_checkout_options checopt = GIT_CHECKOUT_OPTIONS_INIT;
    checopt.checkout_strategy = GIT_CHECKOUT_FORCE | 
                                GIT_CHECKOUT_REMOVE_UNTRACKED | 
                                GIT_CHECKOUT_REMOVE_IGNORED;
    if (git_checkout_tree(repo, obj, &checopt) != 0) {
        QString error = git_get_error();
        git_object_free(obj);
        return Gerror(error);
    }

    if (git_reset(repo, obj, GIT_RESET_HARD, NULL) != 0) {
        QString error = git_get_error();
        git_object_free(obj);
        return Gerror(error);
    }
    
    git_object_free(obj);
    return Gerror();
}

Gerror Repository::log(QList<CommitInfo>& list) const {
    if (repo == NULL) {
        return Gerror("repo is NULL");
    }
    
    list.clear();
    list.reserve(100);
    
    git_revwalk* walker = NULL;
    
    if (git_revwalk_new(&walker, repo) != 0) {
        return Gerror(git_get_error());
    }
    
    git_revwalk_sorting(walker, GIT_SORT_TIME);
    
    git_oid oid_head;
    if (git_reference_name_to_id(&oid_head, repo, "HEAD") != 0) {
        QString error = git_get_error();
        git_revwalk_free(walker);
        return Gerror(error);
    }
    
    if (git_revwalk_push(walker, &oid_head) != 0) {
        QString error = git_get_error();
        git_revwalk_free(walker);
        return Gerror(error);
    }
    
    git_oid oid;
    while (git_revwalk_next(&oid, walker) == 0) {
        git_commit* commit = NULL;
        if (git_commit_lookup(&commit, repo, &oid) != 0) {
            continue; 
        }
        
        char hash_str[GIT_OID_HEXSZ + 1];
        git_oid_tostr(hash_str, sizeof(hash_str), &oid);
        QString commit_hash = QString::fromUtf8(hash_str);
        
        const git_signature* author = git_commit_author(commit);
        QString author_name, author_email;
        QDateTime author_time;
        if (author != NULL) {
            author_name = QString::fromUtf8(author->name);
            author_email = QString::fromUtf8(author->email);
            qint64 time_ms = static_cast<qint64>(author->when.time) * 1000;
            author_time = QDateTime::fromMSecsSinceEpoch(time_ms);
        }
        
        const git_signature* committer = git_commit_committer(commit);
        QString committer_name, committer_email;
        QDateTime committer_time;
        if (committer != NULL) {
            committer_name = QString::fromUtf8(committer->name);
            committer_email = QString::fromUtf8(committer->email);
            qint64 time_ms = static_cast<qint64>(committer->when.time) * 1000;
            committer_time = QDateTime::fromMSecsSinceEpoch(time_ms);
        }
        
        const char* msg_raw = git_commit_message(commit);
        QString msg = msg_raw ? QString::fromUtf8(msg_raw) : "";
        
        list.append(CommitInfo(author_time, author_name, author_email,
                               msg, commit_hash,
                               committer_time, committer_name, committer_email));
        
        git_commit_free(commit);
    }
    
    git_revwalk_free(walker);
    return Gerror();
}
