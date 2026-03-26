#include "../core/git_base.h"
#include "../core/git_repo.h"


int core_repo_commit(git_repository *repo,const char *name,const char *mail,
                     const char *message)
{
    if(repo == NULL){
       return -1; 
    }

    git_index *index = NULL;
    git_signature *signature = NULL;
    git_tree *tree = NULL;
    git_oid commit_id;
   

   return 0;
   
}

int core_repo_push(git_repository *repo,int argc,char *argv[])
{
    return 0;
}