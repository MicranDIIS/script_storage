#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <QDebug>
//сначала собрать солюшен, а после это


//пример callback ф-ии
void func(){
    qDebug() << "rabotaet";
}

static int N = 0;
void func_two(){
    N++;
    qDebug() << "rabotaet" << N;
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
    handler.callback = func_two;
    handler.filePath = filePath;


    IRepository* repo = createRepository(cfg);
    GitError err = repo->open();
    if(err.success){
        qDebug() << err.message << err.code;
    }

    repo->startCheckUpdatesActiveFile(handler, 5);

    return a.exec();
}
