#include "core/core_git.hpp"
#include <iostream>


//argv[1] = URL_repo , argv[2] = local_path , argv[3] = token 
/*int main(int argc,char *argv[]){
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
}*/
const char *path = "/home/user/TEST";

int main(int argc, char *argv[]){
    git_libgit2_init();
    
    git_repository *repo = NULL;

    int error = git_repository_open(&repo, path);
    if(error != 0){
        const git_error *err = git_error_last();
        std::cout << "Ошибка открытия репозитория: " 
                  << (err ? err->message : "unknown error") << std::endl;
        git_libgit2_shutdown();
        return -1;
    }

    std::cout << "Репозиторий открыт: " << path << std::endl;

    
    auto list = core_git_status(repo);

    if(list.isEmpty()){
        std::cout << "Измененных файлов не найдено" << std::endl;
    } else {
        std::cout << "Найдено измененных файлов: " << list.size() << std::endl;
        for(const QString &value : list){
            std::cout << value.toStdString() << std::endl;
        }
    }

    git_repository_free(repo);
    git_libgit2_shutdown();
    return 0;
}