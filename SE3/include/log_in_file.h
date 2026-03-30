#include <QCoreApplication>
#include <QFile>

#ifdef HAVE_QT5
void debugMessageOutput(QtMsgType type,const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    if (type == QtDebugMsg) {
        QString fileName = QCoreApplication::applicationDirPath()+ "/../log_se3.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg.toLatin1().data());
        Gl_file.write("\r\n");
        Gl_file.close();
    }
    if(type==QtWarningMsg)
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/../log_se3.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg.toLatin1().data());
        Gl_file.write("\r\n");
        Gl_file.close();
        exit(2);
    }
    if(type==QtFatalMsg)
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/../log_se3.txt";
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
        QString fileName = QCoreApplication::applicationDirPath() + "/../log.txt";
        QFile Gl_file;
        Gl_file.setFileName(fileName);
        Gl_file.open(QIODevice::Append);
        Gl_file.write(msg);
        Gl_file.write("\r\n");
        Gl_file.close();
    }
    if(type==QtWarningMsg)
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/../log.txt";
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
    default:
        break;
    }
}
