#include "local.h"


QList<FileStatus> Repo::status(){
    QList<FileStatus> list;
    if(repo == NULL){
        error = "пустой указатель на репозиторий";
        return list;
    }

    git_status_list *status = NULL;
    git_status_options statopt = GIT_STATUS_OPTIONS_INIT;
    statopt.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    statopt.flags = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    if(git_status_list_new(&status,repo,&statopt) != 0){
        return list;
    }

    size_t len = git_status_list_entrycount(status);

    list.reserve(len);
    for(size_t i = 0; i <  len;i++){
        QString file_path_new;
        QString file_path_old;
        int file_status = 0;
        const git_status_entry *entry = git_status_byindex(status,i);
        if(entry == NULL || entry -> status & GIT_STATUS_CURRENT){
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
