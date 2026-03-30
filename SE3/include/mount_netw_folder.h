#ifndef MOUNT_NETW_FOLDER_H
#define MOUNT_NETW_FOLDER_H

#include <QProcess>
#include <QObject>
#include <QInputDialog>
#include <../include/findmodules.h>

QString m_upass;

int MountNetworkFolder(QWidget* parent, ModuleFinder*pFinder, QString mountPoint)
{
    if(pFinder->GetNetworkPrefix().isEmpty())
        return -1;

    QDir mPoint(mountPoint);
    if(mPoint.exists() && !mPoint.isEmpty()){
        pFinder->SetNetworkPrefix(mountPoint+QDir::separator());
        return 0;
    }

    QProcess proc;
    m_upass = QInputDialog::getText(parent,"Password",
                                          "It is necessary to mount a network folder.\r\nEnter root password (sudo rights):",QLineEdit::Password);
    QString unname = QInputDialog::getText(parent,"User network name",
                                           "It is necessary to mount a network folder.\r\nEnter shared username:");
    QString unpass = QInputDialog::getText(parent,"Network password",
                                           "It is necessary to mount a network folder.\r\nEnter shared password:",QLineEdit::Password);
    proc.start("/bin/bash");
    proc.waitForStarted(3000);

    QString tcmd = "";
    if(!QFileInfo::exists(mountPoint))
    {
        tcmd = QString("echo %1 | sudo -S mkdir -p %2").arg(m_upass).arg(mountPoint);
        proc.write(tcmd.toUtf8()); // create temp mount directory
        proc.closeWriteChannel();
        proc.waitForFinished(3000);

        if(proc.exitCode()){
            qDebug()<<"mkdir"<<tcmd<<proc.readAllStandardOutput().data();
            return -1;
        }
//        addMessage(D2Color(tr("Create mount point"),"black",tr("SUCCESSFUL"),"green"));
    }

    tcmd = QString("echo %1 | sudo -S mount -t cifs -o vers=1.0,username=%2,password=%3 %4 %5")
            .arg(m_upass)
            .arg(unname)
            .arg(unpass)
            .arg(pFinder->GetNetworkPrefix())
            .arg(mountPoint);

    proc.start("/bin/bash");
    proc.waitForStarted(3000);
    proc.write(tcmd.toUtf8()); // mount network folder to temp directory
    proc.closeWriteChannel();
    proc.waitForFinished(3000);
    if(proc.exitCode()){
        qDebug()<<"mount"<<tcmd<<proc.readAllStandardOutput().data();
        return -1;
    }
//    addMessage(D2Color(tr("Mount network folder"),"black",tr("SUCCESSFUL"),"green"));

    pFinder->SetNetworkPrefix(mountPoint+QDir::separator());
    return 0;
}

int UnMountNetworkFolder(QWidget* parent, QString mountPoint)
{
    QDir mountFolder(mountPoint);
    if(mountFolder.exists() && !mountFolder.isEmpty())
    {
        if(m_upass.isEmpty())
            m_upass = QInputDialog::getText(parent,"Password",
                                            "It is necessary to unmount a network folder.\r\nEnter root password (sudo rights):",QLineEdit::Password);
        QProcess proc;
        QString tcmd = "";
        tcmd = QString("echo %1 | sudo -S umount %2")
                .arg(m_upass)
                .arg(mountPoint);
        proc.start("/bin/bash");
        proc.waitForStarted(3000);
        proc.write(tcmd.toUtf8()); // umount network folder from temp directory
        proc.closeWriteChannel();
        proc.waitForFinished(3000);
        if(proc.exitCode()){
            qDebug()<<"umount"<<tcmd<<proc.readAllStandardOutput().data();
            return -1;
        }
//        addMessage(D2Color(tr("Umount network folder"),"black",tr("SUCCESSFUL"),"green"));

//        tcmd = QString("rm -r %1").arg(m_mountPoint);
//        proc.start("/bin/bash");
//        proc.waitForStarted(3000);
//        proc.write(tcmd.toUtf8()); // umount network folder from temp directory
//        proc.closeWriteChannel();
//        proc.waitForFinished(3000);
//        if(proc.exitCode()){
//            qDebug()<<"remove dir"<<tcmd<<proc.readAllStandardOutput().data();
//            return -1;
//        }
    }
    return 0;
}

#endif // MOUNT_NETW_FOLDER_H
