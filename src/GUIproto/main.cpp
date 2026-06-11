#include <QtGui/QApplication>
#include "mainwindow.h"
#include <mgit.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("MicranDIIS");
    QCoreApplication::setApplicationName("SE2");

    int rc = 0;
    {
        MainWindow w;
        w.show();
        rc = a.exec();
    }

    return rc;
}
