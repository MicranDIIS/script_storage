#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <QDebug>
//сначала собрать солюшен, а после это


//пример callback ф-ии
void func(){
    qDebug() << "rabotaet";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RepoConfig cfg;
    QString filePath = "";
    cfg.url = "";
    cfg.token = "";
    cfg.path = "";
    cfg.branch = "";
    cfg.username = "";

    FileEventHandler handler;
    handler.callback = func;
    handler.filePath = filePath;


    IRepository* repo = createRepository(cfg);
    GitError err = repo->open();
    if(err.success){
        qDebug() << err.message << err.code;
    }

    repo->startCheckUpdatesActiveFile(handler, 0, err);
    repo->stopCheckUpdatesActiveFile();
    return a.exec();
}
