#include <QtGui/QApplication>
#include "mainwindow.h"
#include <mgit.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("MicranDIIS");
    QCoreApplication::setApplicationName("SE2");

//    mgit_init();

    QTranslator translator;
    translator.load("C:/SE2orig/script_storage/GUIproto/app_en.qm"); // maybe need to move it to release folder
    a.installTranslator(&translator);

    int rc = 0;
    {
        MainWindow w;
        w.show();
        rc = a.exec();
    }

//    mgit_shutdown();
    return rc;
}
