#ifndef CC_GENERATE_H
#define CC_GENERATE_H
#include <QString>
#include <QDir>
#include <QProcess>
#include <QStringList>

void CodeCoverage()
{
    QString codeCoverBAT = QString("%1%2").arg(PROJ_DIR).arg("tools/code_covarage/lcov.bat");
    QProcess::startDetached("cmd",QStringList()<<QString("/c %1").arg(codeCoverBAT),OBJ_DIR);
}

#endif // CC_GENERATE_H
