#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <qDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RepoConfig cfg;
    cfg.path = "";
    IRepository* repo = createRepository(cfg);
    Gerror err = repo->open();
    if(err.hasError()){
        qDebug() << err.getMsg();
        return a.exec();
    }else{
        qDebug() << "Repo is open";
    }
    deleteRepository(repo);

    return a.exec();
}
