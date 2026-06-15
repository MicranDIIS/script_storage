#include <QtGui/QApplication>
#include "mainwindow.h"
#include <mgit.h>
#include <QTranslator>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("MicranDIIS");
    QCoreApplication::setApplicationName("SE2");

//    mgit_init();

    QTranslator translator;

    if (!translator.load(":/translations/app_ru.qm"))
        qDebug() << "Failed to load translation from resources";
    else
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
