#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <qDebug>
#include <stdio.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mgit_init();
    
    RepoConfig cfg;
    cfg.branch = "backend";
    //свое
    cfg.path = "";
    //свое
    cfg.token = "";
    cfg.url = "https://github.com/MicranDIIS/script_storage";
    //свое
    cfg.username = "";

    IRepository* repo = createRepository(cfg);
    Gerror err = repo->clone();
    if(!err.succses){
        qDebug() << err.msg;
        return a.exec();
    }else{
        printf("good clone!\n");
    }

    deleteRepository(repo);

    cfg.branch = "main";
    IRepository *repo_ = createRepository(cfg);
    err = repo->open();
    if(!err.succses){
            qDebug() << err.msg;
            return a.exec();
    }else{
        printf("open is branch main\n");
    }


    deleteRepository(repo_);
    mgit_shutdown();
    return a.exec();
}
