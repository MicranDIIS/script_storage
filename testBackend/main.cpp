#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RepoConfig cfg;
    cfg.path = "";

    IRepository *repo = createRepository(cfg);
    GitError err = repo->open();
    if(!err.success){
        qDebug() << err.message;
    }

    bool check = repo->isValidRepo();
    if(!check){
        qDebug() << "repo is not valid";
    } else {
        qDebug() << "repo is valid";
    }


    return a.exec();
}
