#include "../core/core_git.hpp"

void core_start(){
    git_libgit2_init();
}

void core_close(){
    git_libgit2_shutdown();
}