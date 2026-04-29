#include <QtCore/QCoreApplication>
#include <network.h>
#include <local.h>
#include <basedata.h>
#include <stdio.h>
#include <QDebug>

int main(int argc, char *argv[])
{
<<<<<<< HEAD

    setlocale(LC_ALL,"Russian");
=======
>>>>>>> origin/feat/33-git-reset
    QCoreApplication a(argc, argv);

    git_libgit2_init();

<<<<<<< HEAD

    /*Repo repo = Repo::clone("https://github.com/MicranDIIS/script_storage","","","");
    if(!repo.valid()){
        printf("error\n");
    }else{
        printf("yes\n");
    }

    if(!repo.valid()){
        printf("no valid");
    }else{
        printf("yra");
    }
    FILE *f = fopen("\\test.txt","w");
    fprintf(f,"%s","asd");


    QList<FileStatus> list = repo.status();


    if(list.isEmpty()){
        printf("pysto");
    }
    foreach(FileStatus value,list){
        if(value.flag_check(STATUS_NEW_TO_DIR)){
            qDebug() << value.get_path_new() + " " + "NEW_TO_DIR";
        }
    }*/

=======
    //путь надо на свой поменять
>>>>>>> origin/feat/33-git-reset
    Repo repo = Repo::open("C:\\Users\\user\\testing","");

    if(!repo.valid()){
        printf("no valid!");
<<<<<<< HEAD
    }else{
        if(repo.fetch() != -1){
            printf("yes!");
        }else{
            printf("no!");
        }

    }

    git_libgit2_shutdown();

    //fclose(f);
=======
        return a.exec();
    }else{
        printf("valid!\n");
    }


    if(repo.reset() != 0){
        printf("error!");
    }else{
        printf("norm");
    }


    git_libgit2_shutdown();

>>>>>>> origin/feat/33-git-reset
    return a.exec();
}
