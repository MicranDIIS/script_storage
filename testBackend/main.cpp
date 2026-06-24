#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <QDebug>
#include <QMetaType>
//сначала собрать солюшен, а после это


//пример callback ф-ии
void func(){
    qDebug() << "rabotaet";
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<size_t>("size_t");

    RepoConfig cfg;
    cfg.url = "";
    cfg.token = "";
    cfg.path = "";
    cfg.branch = "";
    cfg.username = "";

    IRepository* repo = createRepository(cfg);
    GitError err = repo->open();
    if(!err.success){
        qDebug() << err.message;
        return -1;
    }

    qDebug() << "norm";


    repo->setCallbackNotification(*func);
    //время в секундах, а путь относительно .git к файлу
    repo->startCheckUpdatesActiveFile(1, "");

    return a.exec();
}
