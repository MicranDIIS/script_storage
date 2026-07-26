#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <QDebug>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RepoConfig cfg;
    cfg.path = "";
    cfg.branch = "main";

    IRepository *repo = createRepository(cfg);
    GitError err = repo->open();
    if(!err.success){
        qDebug() << err.message;
    }

    bool check = repo->isValidRepo();
    if(!check){
        qDebug() << "repo is not valid";
    }else{
        qDebug() << "repo is valid";
    }

    err = repo->startDebugMode();
    if(!err.success){
        qDebug() << err.message;
    }

    FILE *f = fopen("","w");
    fprintf(f, "%s", "asd");
    fclose(f);

    err = repo->saveDebugFiles("save debug");
    if(!err.success){
        qDebug() << err.message;
    }


    err = repo->closeDebugMode();
    if(!err.success){
        qDebug() << err.message;
    }

    return a.exec();
}
