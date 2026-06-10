#include "repository.h"


Gerror Repository::open(){
    QByteArray path = cfg_.path.toUtf8();

    if(git_repository_open(&repo_, path.constData()) != GIT_OK){
        return libgitError();
    }

    return Gerror();
}

Gerror Repository::status(QList<FileStatus>& list) const {
    if (repo_ == NULL) {
        return Gerror("repo is NULL",REPO_IS_NULL);
    }

    git_status_list* status = NULL;
    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    if (git_status_list_new(&status, repo_, &statopt) != GIT_OK) {
        return libgitError();
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
    if (repo_ == NULL) {
        return Gerror("repo is NULL", REPO_IS_NULL);
    }
    
    git_object* obj = NULL;
    
    if (git_revparse_single(&obj, repo_, "HEAD") != GIT_OK) {
        return libgitError();
    }
    
    git_checkout_options checopt = GIT_CHECKOUT_OPTIONS_INIT;
    checopt.checkout_strategy = GIT_CHECKOUT_FORCE | 
                                GIT_CHECKOUT_REMOVE_UNTRACKED | 
                                GIT_CHECKOUT_REMOVE_IGNORED;
    if (git_checkout_tree(repo_, obj, &checopt) != GIT_OK) {
        git_object_free(obj);
        return libgitError();
    }

    if (git_reset(repo_, obj, GIT_RESET_HARD, NULL) != GIT_OK) {
        git_object_free(obj);
        return libgitError();
    }
    
    git_object_free(obj);
    return Gerror();
}

Gerror Repository::log(QList<CommitInfo>& list) const {
    if (repo_ == NULL) {
        return Gerror("repo is NULL",REPO_IS_NULL);
    }
    
    list.clear();
    list.reserve(10);
    
    git_revwalk* walker = NULL;
    
    if (git_revwalk_new(&walker, repo_) != GIT_OK) {
        return libgitError();
    }
    
    git_revwalk_sorting(walker, GIT_SORT_TIME);
    
    git_oid oid_head;
    if (git_reference_name_to_id(&oid_head, repo_, "HEAD") != GIT_OK) {
        git_revwalk_free(walker);
        return libgitError();
    }
    
    if (git_revwalk_push(walker, &oid_head) != GIT_OK) {
        git_revwalk_free(walker);
        return libgitError();
    }
    
    git_oid oid;
    while (git_revwalk_next(&oid, walker) == GIT_OK) {
        git_commit* commit = NULL;
        if (git_commit_lookup(&commit, repo_, &oid) != 0) {
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

struct LogFile {
    QString file_path;
    bool found;
};

static int diff_file_callback(const git_diff_delta* delta, float progress, 
                              void* payload)
{
    (void)progress;
    LogFile* file = static_cast<LogFile*>(payload);
    
    const QString old_path = delta->old_file.path ? QString::fromUtf8(delta->old_file.path) : QString();
    const QString new_path = delta->new_file.path ? QString::fromUtf8(delta->new_file.path) : QString();
    
    if (old_path == file->file_path || new_path == file->file_path) {
        file -> found = true;
        return 1; 
    }
    
    return 0;
}

Gerror Repository::log(QList<CommitInfo>& list, const QString& filePath) const{
    if (repo_ == NULL) {
        return Gerror("repo is NULL", REPO_IS_NULL);
    }

    list.clear();
    list.reserve(10);

    git_revwalk* walker = NULL;
    
    if (git_revwalk_new(&walker, repo_) != GIT_OK) {
        return libgitError();
    }
    
    git_revwalk_sorting(walker, GIT_SORT_TIME);
    
    git_oid oid_head;
    if (git_reference_name_to_id(&oid_head, repo_, "HEAD") != GIT_OK) {
        git_revwalk_free(walker);
        return libgitError();
    }
    
    if (git_revwalk_push(walker, &oid_head) != GIT_OK) {
        git_revwalk_free(walker);
        return libgitError();
    }
    
    git_oid oid;
    while (git_revwalk_next(&oid, walker) == GIT_OK) {
        git_commit* commit = NULL;
        if (git_commit_lookup(&commit, repo_, &oid) != GIT_OK) {
            continue;
        }

        git_tree* tree = NULL;
        if (git_commit_tree(&tree, commit) != GIT_OK) {
            git_commit_free(commit);
            continue;
        }

        git_tree* tree_parent = NULL;
        if (git_commit_parentcount(commit) > 0) {
            git_commit* parent = NULL;
            if (git_commit_parent(&parent, commit, 0) != GIT_OK) {
                git_tree_free(tree);
                git_commit_free(commit);
                continue;
            }
            if (git_commit_tree(&tree_parent, parent) != GIT_OK) {
                git_commit_free(parent);
                git_tree_free(tree);
                git_commit_free(commit);
                continue;
            }
            git_commit_free(parent);
        }

        QByteArray file_path = filePath.toUtf8();
        const char* file_path_ = file_path.constData();
        
        git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
        opts.pathspec.strings = (char**)&file_path_;
        opts.pathspec.count = 1;

        git_diff* diff = NULL;
        if (git_diff_tree_to_tree(&diff, repo_, tree_parent, tree, &opts) != GIT_OK) {
            git_tree_free(tree_parent);
            git_tree_free(tree);
            git_commit_free(commit);
            continue;
        }

        LogFile file = {filePath, false};

        if(git_diff_foreach(diff, diff_file_callback, NULL, NULL, NULL, &file) < 0){
            git_diff_free(diff);
            git_tree_free(tree_parent);
            git_tree_free(tree);
            git_commit_free(commit);
            continue;
        }

        if (file.found) {
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
        }

        git_diff_free(diff);
        git_tree_free(tree_parent);
        git_tree_free(tree);
        git_commit_free(commit);
    }

    git_revwalk_free(walker);
    return Gerror();
}