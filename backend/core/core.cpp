#include "core.h"

void core_init(void){
    git_libgit2_init();
}

void core_shutdown(){
    git_libgit2_shutdown();
}


