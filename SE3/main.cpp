#include <QApplication>
#include <QtCore>
#include "se3.h"
#include <QTextStream>
#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif
#include "../include/log_in_file.h"
#include "stdlib.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);
    qputenv("PATH",QString("%1;%2/../libs").arg(QString(qgetenv("PATH"))).arg(a.applicationDirPath()).toLocal8Bit());
    qputenv("PATH",QString("%1;%2/../libs/extLibs").arg(QString(qgetenv("PATH"))).arg(a.applicationDirPath()).toLocal8Bit());
    qputenv("PYTHONPATH",QString("%1/PythonLib").arg(a.applicationDirPath()).toLocal8Bit());
    qputenv("PYTHONHOME",NULL);
    qputenv("PYTHONDONTWRITEBYTECODE",QString("1").toLocal8Bit().data());
    qDebug()<<getenv("LD_LIBRARY_PATH");
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    // install translates
    QDir::setCurrent(QCoreApplication::applicationDirPath()+"/..");
    QDir langDir(QCoreApplication::applicationDirPath() + "/translations/","*.qm");
    QStringList filtersList;
    filtersList << "*.qm";
    QFileInfoList fileNames = langDir.entryInfoList(filtersList,QDir::Files);
    QTranslator trans[fileNames.size()];
    for (int i = 0; i < fileNames.size(); i++) {
        trans[i].load(fileNames.at(i).absoluteFilePath());
        a.installTranslator(&trans[i]);
    }

    // analize arguments
    bool editStorage = false;
    QString scriptPath = "";
    for(int i = 0; i < argc; i++){
        QString parameter;
        parameter = argv[i];
        if (parameter == "-d") {
#ifdef HAVE_QT5
            qInstallMessageHandler(debugMessageOutput);
#else
            qInstallMsgHandler(debugMessageOutput);
#endif
            qDebug()<<"<--------- New session /// "<<QDateTime::currentDateTime().toLocalTime().toString()<<" --------->";
        }
        else if (parameter == "-es") {
            editStorage = true;
        }
        else if(parameter.contains(".lua")){
            int s = i;
            QString ppath="";
            while(s)
            {
                ppath=argv[s]+QString(" ")+ppath;
                s--;
            }
            parameter = ppath.trimmed();

            QFileInfo fInfo(parameter);
            if (fInfo.isFile()) {
                scriptPath = parameter;
            }
        }
    }

#ifdef TEST_MODE
        qDebug()<<"TEST_MODE";
       return runTest(argc, argv);
#endif
    // ----------------------------- //
    SE3 w(editStorage);
//    w.setEditStorage(editStorage);
    // ----------------------------- //

    // run script direct or launch manager
    if (scriptPath.isEmpty()) {
        w.show();
        w.init();
    } else
    {
        QEventLoop el;
        QTimer::singleShot(100,&el,SLOT(quit()));
        el.exec();
        w.RunScriptFromConsole(scriptPath);
    }
//    exit(-1);
    return a.exec();
}
