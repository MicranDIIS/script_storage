#include <QApplication>
#include <QtCore>
#include "se2.h"
#include <QTextStream>
#include <conio.h>
#include <QMessageBox>
//#include <mpatrol.h>


#ifdef HAVE_QT5
void debugMessageOutput(QtMsgType type,const QMessageLogContext &context, const QString &msg)
{
    if (type == QtDebugMsg) {
        QString fileName = QApplication::applicationDirPath() + "/log.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg.toLatin1().data());
        Gl_file.write("\r\n");
        Gl_file.close();
    }
    if(type==QtWarningMsg)
    {
        QString fileName = QApplication::applicationDirPath() + "/log.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg.toLatin1().data());
        Gl_file.write("\r\n");
        Gl_file.close();
        exit(2);
    }
}
#else
void debugMessageOutput(QtMsgType type, const char *msg)
{
    if (type == QtDebugMsg) {
        QString fileName = QApplication::applicationDirPath() + "/log.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg);
        Gl_file.write("\r\n");
        Gl_file.close();
    }
    if(type==QtWarningMsg)
    {
        QString fileName = QApplication::applicationDirPath() + "/log.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg);
        Gl_file.write("\r\n");
        Gl_file.close();
//        exit(2);
    }
}
#endif

void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        exit(-1);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        exit(1);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

void  CheckLauncherUpdate()
{
    qDebug()<<"startLauncher";
     QSettings localSettings(QCoreApplication::applicationDirPath() + "/localSettings.ini", QSettings::IniFormat);
     localSettings.setIniCodec(QTextCodec::codecForName("Windows-1251"));
     QString storageLocation = localSettings.value("storageLocation").toString();
     if (storageLocation.lastIndexOf("/") != (storageLocation.size() - 1))
	 storageLocation += "/";
     if( !QFileInfo(storageLocation + "remoteSettings.ini").exists())
     {
	  QMessageBox::critical(0,QObject::tr("IMPORTANT"),QObject::tr("Remote storage - not available"));
	  return;
     }
     QSettings remoteSettings(storageLocation + "remoteSettings.ini", QSettings::IniFormat);
     QByteArray localHash = localSettings.value("files/Launcher.exe").toByteArray();
     QByteArray remoteHash = remoteSettings.value("files/Launcher.exe").toByteArray();
     if(localHash!=remoteHash || localHash==QVariant())
     {
	 qDebug()<<"hashes are diff";
	 QMessageBox::StandardButton reply;
	 reply = QMessageBox::question(0, "INFORMATION", "Launcher update available\r\nUpdate ?",
					 QMessageBox::Ok | QMessageBox::Cancel);

	 if (reply != QMessageBox::Ok) {
	     return;
	 }

	 QString localRoot = QCoreApplication::applicationDirPath() + QDir::separator();
	 QString localFilePath = localRoot+"Launcher.exe";
	 QString remoteFilePath = storageLocation+"Launcher.exe";

	 QFile remoteFile(remoteFilePath);
	 QFile localFile(localFilePath);
	 if (localFile.exists()) {
	     if (! localFile.remove()){
		 QMessageBox::critical(0,QObject::tr("IMPORTANT"),QObject::tr("Launcher remove - fail"));
	     }
	 }

	 if (! remoteFile.copy(localFilePath)){
	     QMessageBox::critical(0,QObject::tr("IMPORTANT"),QObject::tr("Launcher update - fail"));
	 }
	localSettings.setValue("files/Launcher.exe",remoteHash);
	QMessageBox::information(0,QObject::tr("IMPORTANT"),QObject::tr("Launcher update - successfull\r\nProgramm will be restart"));

//	 QFileInfo selfUpdater(localRoot + "selfUpdate.bat");
//	 if(!selfUpdater.exists()){
//	     QMessageBox::information(0,"Information","selfUpdate module not found !!!");
//	     return;
//	 }

	 QProcess proc;
	 if (!proc.startDetached(localFilePath)){
		QMessageBox::critical(0,"Unable rerun Launcher",proc.errorString());
		return;
	 }
	 else
	     exit(0);

//	 QString argS = QString("/c %1 %2 %3")
//		 .arg(QDir::toNativeSeparators(QDir::cleanPath(selfUpdater.absoluteFilePath())))
//		 .arg(QDir::toNativeSeparators(QDir::cleanPath(updateLink)))
//		 .arg(QDir::toNativeSeparators(QDir::cleanPath(localFilePath)));

//	 proc.setWorkingDirectory(QApplication::applicationDirPath());
//	 if (!proc.startDetached("cmd",QStringList()<<argS)){
//	      QMessageBox::critical(0,"SelfUpdate error",proc.errorString());
//	     return;
//	 }
//	 else
//	 {
////	     bQuit = true;
////	     QMessageBox::information(0,"IMPORTANT","Init selfupdate.. The programm will be restarted");
////	     ui->textBrowser->insertHtml(tr("<b color=\"red\">Init selfupdate.. The programm will be restarted</b><br>"));
//	     QTimer::singleShot(2000,qApp,SLOT(quit()));
//	     localSettings.setValue("files/Launcher.exe",remoteHash);
//	 }
     }
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QApplication a(argc, argv);
    QDir langDir(QCoreApplication::applicationDirPath() + "/lang/","*.qm");
    QStringList filtersList;
    filtersList << "*.qm";
    QFileInfoList fileNames = langDir.entryInfoList(filtersList,QDir::Files);
    QTranslator trans[fileNames.size()];
    for (int i = 0; i < fileNames.size(); i++) {
        trans[i].load(fileNames.at(i).absoluteFilePath());
        a.installTranslator(&trans[i]);
    }

    QSettings settings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
        settings.setValue(".uod/.", "SE2");
        settings.setValue("SE2/.", "Script Executor v2");
        settings.setValue("SE2/shell/open/command/.", "\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\"" + " \"%1\"");
        settings.setValue("SE2/DefaultIcon/.", QString("%1,1").arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath())));

    QString scriptPath = "";
    bool bDirectRun = true;
    bool bLauncherUpdate =true;



    for(int i = 0; i < argc; i++){
        QString parameter;
        parameter = argv[i];
        if (parameter == "-d") {
            QString fileName = QApplication::applicationDirPath() + "/log.txt";
            QFileInfo info(fileName);
            if (info.exists()) {
                QFile file(fileName);
                file.remove();
            }
#ifdef HAVE_QT5
            qInstallMessageHandler(debugMessageOutput);
#else
            qInstallMsgHandler(debugMessageOutput);
#endif

//            qInstallMsgHandler(myMessageOutput);
	}
	else if(parameter == "-l")
	{
	    bDirectRun = false;
	}
	else if(parameter == "-x")
	{
	    bLauncherUpdate = false;
	}
	else if (i > 1) {

            if(parameter.contains(".lua")){
//                int s = i;
//                QString ppath="";
//                while(s)
//                {
//                    ppath=argv[s]+QString(" ")+ppath;
//                    s--;
//                }
		parameter = parameter.trimmed();
            }
	    qDebug()<<"lua_file"<<parameter;

            QFileInfo fInfo(parameter);
            if (fInfo.isFile()) {
                scriptPath = parameter;
            }
        }
    }

    if(bLauncherUpdate)
	CheckLauncherUpdate();

    if(bDirectRun){
	QMessageBox::critical(0,QObject::tr("Incorrect launch way"),QObject::tr("Please run Launcher"));
	return 0;
    }


    SE2 w;
    if (scriptPath.isEmpty()) {
        w.show();
        return a.exec();
    } else {
        QString specific;
        QString category;
        if (w.getScriptHeader(scriptPath, specific, category)) {
            w.setScriptPath(scriptPath);
            w.setCategory(category);
            w.setSpecific(specific);
            QTimer::singleShot(100,&w,SLOT(runScript()));
            return a.exec();
        } else {
            return 0;
        }
    }





}
