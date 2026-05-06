#include <QtCore/QCoreApplication>
#include <mgit.h>
#include <qDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mgit_init();
    

    mgit_shutdown();
    return a.exec();
}
