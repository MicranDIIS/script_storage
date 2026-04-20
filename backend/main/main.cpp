#include <QtCore/QCoreApplication>
#include <network.h>
#include <local.h>
#include <basedata.h>
#include <stdio.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    git_libgit2_init();

    //путь надо на свой поменять
    Repo repo = Repo::open("C:\\Users\\user\\testing","");

    if(!repo.valid()){
        printf("no valid!");
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

    return a.exec();
}
