#include "core/core_git.hpp"
#include <iostream>


//argv[1] = URL_repo , argv[2] = local_path , argv[3] = token 
int main(int argc,char *argv[]){
    if(argc != 4){
        std::cout << "Введено слишком мало аргументов!" << std::endl;
    }

    git_libgit2_init();

    if(core_git_clone(argv[1],argv[2],argv[3]) != 0){
        std::cout << "Ошибка" << std::endl;
        return -1;
    }

    std::cout << "Репозиторий клонирован" << std::endl;

    git_libgit2_shutdown();
    return 0;
}