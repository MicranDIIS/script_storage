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