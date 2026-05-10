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

    IRepository *repo = createRepository(cfg);

    Gerror err = repo->open();
    if(!err.succses){
        qDebug() << err.msg;
    }else{
        printf("good\n");
    }

    QList<CommitInfo> list;
    err = repo->log(list, "backend/include/MGit/mgit.h");
    if(!err.succses){
        qDebug() << err.msg;
    }else{
        foreach (const CommitInfo& ci, list) {
            qDebug() << "commit" << ci.commitHash;
            qDebug() << "Author:" << ci.authorName << "<" << ci.authorEmail << ">";
            qDebug() << "Date:  " << ci.authorDateTime.toString("yyyy-MM-dd HH:mm:ss");
            qDebug() << "";
            qDebug() << "    " << ci.commitMsg.trimmed();
            qDebug() << "";
        }
    }


    deleteRepository(repo);
    mgit_shutdown();
    return a.exec();
}
