#include "repository.h"


IRepository* createRepository(const RepoConfig& cfg_){
    return new Repository(cfg_);
}

void deleteRepository(IRepository *repo){
    delete repo;
}

void mgit_init(){
    git_libgit2_init();
}
void mgit_shutdown(){
    git_libgit2_shutdown();
}


int repo_remote_branch_create_to_local(git_repository* repo, const QString& branch){
    git_reference *ref = NULL;
    QString remote_ref_name  = QString("refs/remotes/origin/%1").arg(branch);
    QByteArray remote_ref_name_ = remote_ref_name.toUtf8();
    if(git_reference_lookup(&ref, repo, remote_ref_name_.constData()) != 0){
        return -1;
    }

    git_object *obj = NULL;
    if(git_reference_peel(&obj, ref, GIT_OBJECT_COMMIT) != 0){
            git_reference_free(ref);
            return -1;
        }

    git_reference *local_ref = NULL;
    QByteArray branch_name = branch.toUtf8();
    if(git_branch_create(&local_ref, repo, branch_name.constData(), 
                     (git_commit*)obj, 0) != 0){
        git_reference_free(ref);
        git_object_free(obj);
        return -1;
    }

    git_object_free(obj); 
    git_reference_free(ref);
    git_reference_free(local_ref);  
    return 0;

}

int repo_checkout_local_branch(git_repository* repo, const QString& branch){
    git_object *obj = NULL;
    QByteArray branch_ = branch.toUtf8();
    if(git_revparse_single(&obj,repo,branch_.constData()) != 0){
        return -1;
    }

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    if(git_checkout_tree(repo, obj, &opts) != 0){
        git_object_free(obj);
        return -1;
    }

    QString branch_path = QString("refs/heads/%1").arg(branch);
    QByteArray branch_path_ = branch_path.toUtf8();
    if(git_repository_set_head(repo,branch_path_.constData()) != 0){
        git_object_free(obj);
        return -1;
    }

    git_object_free(obj);
    return 0;
}
