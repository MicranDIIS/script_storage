#include "repository.h"

GitError Repository::open(){
    if(git_repository_open(&repo_, cfg_.path.constData()) != GIT_OK){
        return libgitError();
    }

    return GitError();
}

GitError Repository::fillStatus(QList<FileStatus>& list) const {
    if (repo_ == NULL) {
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    GitStatusListPtr status;
    if (git_status_list_new(&status, repo_, &statopt) != GIT_OK) {
        return libgitError();
    }

    size_t len = git_status_list_entrycount(status.get());
    list.clear();
    list.reserve(len);
    
    for (size_t i = 0; i < len; i++) {
        FileStatus stat;
        bool check = false;
        const git_status_entry* entry = git_status_byindex(status.get(), i);
        
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
                stat.pathNew = QString::fromUtf8(entry->head_to_index->new_file.path);

                stat.newToHead = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_INDEX_RENAMED) {
                stat.pathNew = QString::fromUtf8(entry->head_to_index->new_file.path);
                stat.pathOld = QString::fromUtf8(entry->head_to_index->old_file.path);

                stat.renameToHead = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_INDEX_DELETED) {
                stat.pathOld = QString::fromUtf8(entry->head_to_index->old_file.path);

                stat.deleteToHead = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_INDEX_MODIFIED) {
                stat.pathNew = QString::fromUtf8(entry->head_to_index->new_file.path);
                stat.pathOld = stat.pathNew;

                stat.modFileToHead = true;
                check = true;
            }
        }

        if (entry->index_to_workdir) {
            if (entry->status & GIT_STATUS_WT_NEW) {
                stat.pathNew = QString::fromUtf8(entry->index_to_workdir->new_file.path);

                stat.newToDir = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_WT_RENAMED) {
                stat.pathNew = QString::fromUtf8(entry->index_to_workdir->new_file.path);
                stat.pathOld = QString::fromUtf8(entry->index_to_workdir->old_file.path);
                
                stat.renameToDir = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_WT_DELETED) {
                stat.pathOld = QString::fromUtf8(entry->index_to_workdir->old_file.path);
                
                stat.deleteToDir = true;
                check = true;
            }
            if (entry->status & GIT_STATUS_WT_MODIFIED) {
                stat.pathNew = QString::fromUtf8(entry->index_to_workdir->new_file.path);
                stat.pathOld = stat.pathNew;
                
                stat.modFileToDir = true;
                check = true;
            }
        }

        if (check) {
            list.append(stat);
        }
    }

    return GitError();
}

GitError Repository::reset() {
    if (repo_ == NULL) {
        return GitError("repo is NULL", REPO_IS_NULL);
    }
    
    GitObjectPtr obj;
    
    if (git_revparse_single(&obj, repo_, HEAD) != GIT_OK) {
        return libgitError();
    }
    
    git_checkout_options checopt = GIT_CHECKOUT_OPTIONS_INIT;
    checopt.checkout_strategy = GIT_CHECKOUT_FORCE | 
                                GIT_CHECKOUT_REMOVE_UNTRACKED | 
                                GIT_CHECKOUT_REMOVE_IGNORED;
    if (git_checkout_tree(repo_, obj.get(), &checopt) != GIT_OK) {
        return libgitError();
    }

    if (git_reset(repo_, obj.get(), GIT_RESET_HARD, NULL) != GIT_OK) {
        return libgitError();
    }
    
    return GitError();
}

static CommitInfo getCommitInfo(const GitCommitPtr& commit, const git_oid* oid){
    char hash_str[GIT_OID_HEXSZ + 1];
    git_oid_tostr(hash_str, sizeof(hash_str), oid);
    QString commit_hash = QString::fromUtf8(hash_str);
        
    const git_signature* author = git_commit_author(commit.get());
    QString author_name, author_email;
    QDateTime author_time;
    if (author != NULL) {
        author_name = QString::fromUtf8(author->name);
        author_email = QString::fromUtf8(author->email);
        qint64 time_ms = static_cast<qint64>(author->when.time) * 1000;
        author_time = QDateTime::fromMSecsSinceEpoch(time_ms);
    }
        
    const char* msg_raw = git_commit_message(commit.get());
    QString msg = msg_raw ? QString::fromUtf8(msg_raw) : "";

    return CommitInfo(author_name, author_email, msg,
                      commit_hash, author_time);
}

GitError Repository::fillLog(QList<CommitInfo>& list) const {
    if (repo_ == NULL) {
        return GitError("repo is NULL", REPO_IS_NULL);
    }
    
    list.clear();
    list.reserve(DEFAULT_SIZE_LIST_LOG);
    
    GitRevwalkPtr walker;
    GitError err = GitRevwalkInit(walker);
    if(!err.success){
        return err;
    }
    
    git_oid oid;
    while (git_revwalk_next(&oid, walker.get()) == GIT_OK) {
        GitCommitPtr commit;
        if (git_commit_lookup(&commit, repo_, &oid) != GIT_OK) {
            continue; 
        }

        list.append(getCommitInfo(commit, &oid));
    }
    
    return GitError();
}

GitError Repository::fillLog(QList<CommitInfo>& list, const QString& filePath) const {
    if (repo_ == NULL) {
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    list.clear();
    list.reserve(DEFAULT_SIZE_LIST_LOG);

    GitRevwalkPtr walker;
    GitError err = GitRevwalkInit(walker);
    if(!err.success){
        return err;
    }
    
    git_oid oid;
    while (git_revwalk_next(&oid, walker.get()) == GIT_OK) {
        GitCommitPtr commit;
        if (git_commit_lookup(&commit, repo_, &oid) != GIT_OK) {
            continue;
        }

        GitTreePtr tree;
        if (git_commit_tree(&tree, commit.get()) != GIT_OK) {
            continue;
        }

        GitTreePtr tree_parent;
        if (git_commit_parentcount(commit.get()) > 0) {
            GitCommitPtr parent;
            if (git_commit_parent(&parent, commit.get(), 0) != GIT_OK) {
                continue;
            }
            if (git_commit_tree(&tree_parent, parent.get()) != GIT_OK) {
                continue;
            }
        }

        QByteArray file_path = filePath.toUtf8();
        const char* file_path_ = file_path.constData();
        
        git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
        opts.pathspec.strings = (char**)&file_path_;
        opts.pathspec.count = 1;

        GitDiffPtr diff;
        if (git_diff_tree_to_tree(&diff, repo_, tree_parent.get(), tree.get(), &opts) != GIT_OK) {
            continue;
        }

        LogFile file = {filePath, false};

        if (git_diff_foreach(diff.get(), diff_file_callback, NULL, NULL, NULL, &file) < 0) {
            if (!file.found) {
                continue;
            }
        }

        if (file.found) {
            list.append(getCommitInfo(commit, &oid));
        }
    }

    return GitError();
}
