#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <qDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mgit_init();

    IRepository *repo = createRepository();
    Gerror err = repo->open("");
    if(!err.succses){
        qDebug() << err.msg << "\n";
    }else{
        qDebug() << "Все хорошо" << "\n";
    }

    mgit_shutdown();
    return a.exec();
}
