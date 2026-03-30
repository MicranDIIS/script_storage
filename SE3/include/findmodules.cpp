#include "findmodules.h"
#define APPFOLDER QCoreApplication::applicationDirPath()
ModuleFinder::ModuleFinder() : ErrorManager()
{
    QSettings loadState(APPFOLDER + "config/localSettings.ini",
                        QSettings::IniFormat);
    loadState.setIniCodec("UTF-8");
    m_libsRelPath = loadState.value("libsRelativeLocation","libs/").toString();
    m_pluginsRelPath = loadState.value("pluginsRelativeLocation","plugins/").toString();
    m_coreStorageAbsPath = loadState.value("coreStorageLocation","//k118/").toString();
    m_networkPrefix = loadState.value("storageNetworkAddr","").toString();
    m_scriptsStorageAbsPath = loadState.value("scriptsStorageLocation","//k118/").toString();
    if(m_coreStorageAbsPath.lastIndexOf("/") != m_coreStorageAbsPath.length()-1)
        m_coreStorageAbsPath.append("/");
    if(m_scriptsStorageAbsPath.lastIndexOf("/") != m_scriptsStorageAbsPath.length()-1)
        m_scriptsStorageAbsPath.append("/");
    if(!m_coreStorageAbsPath.contains("/windows") && !m_coreStorageAbsPath.contains("/linux"))
    {
        QString sysPostfix;
        #ifdef WIN32
        sysPostfix = "windows/";
        #else
        sysPostfix = "linux/";
        #endif
        m_coreStorageAbsPath.append(sysPostfix);
    }
    QDir dir(APPFOLDER);
    m_absAppPath = dir.absolutePath();
    if(m_absAppPath.at(m_absAppPath.length()-1)!='/')
        m_absAppPath.append("/");
//    m_scriptsStorageAbsPath = QDir::toNativeSeparators(QDir::cleanPath(m_scriptsStorageAbsPath));
}

ModuleFinder::~ModuleFinder()
{
}

int ModuleFinder::FindModule(QString name, QString *findedPath)
{
    // check plugins dir
    QDir dirPlug(m_absAppPath + m_pluginsRelPath);
    qDebug()<<m_absAppPath + m_pluginsRelPath;
    if(!dirPlug.exists()){
//        SetError(tr("Plugin`s directory not exist"));
        return MAIN_PDIR_MISS;
    }

    // check exist file
    QFileInfo fileObjPlug(QString("%1/%2").arg(dirPlug.absolutePath()).arg(name));
    if(fileObjPlug.exists())
    {
        *findedPath = fileObjPlug.absoluteFilePath();
        return SE3_NO_ERROR;
    }

    // check libs dir
    QDir dirLib(m_absAppPath + m_libsRelPath);

    if(!dirLib.exists()){
        return  SetError(MAIN_LDIR_MISS);
    }

    // check exist file
    QFileInfo fileObjLib(QString("%1/%2").arg(dirLib.absolutePath()).arg(name));
    if(fileObjLib.exists())
    {
        *findedPath = fileObjLib.absoluteFilePath();
        return SE3_NO_ERROR;
    }

    QFileInfo fileObjLibExt(QString("%1/extLibs/%2").arg(dirLib.absolutePath()).arg(name));
    if(fileObjLibExt.exists())
    {
        *findedPath = fileObjLibExt.absoluteFilePath();
        return SE3_NO_ERROR;
    }

    // check network storage plugins dir

    QDir dirPlugNetw(m_coreStorageAbsPath + m_pluginsRelPath);
    if(!dirPlugNetw.exists()){
        return SetError(tr("Module %1 not found").arg(name));
    }

    QDir fileDirPlugNetw(QString("%1/%2").arg(dirPlugNetw.absolutePath()).arg(name));
    if(fileDirPlugNetw.exists())
    {
        QFileInfo fileObjPlugNetw = FindLastModifFile(fileDirPlugNetw.absolutePath());
        if(fileObjPlugNetw != QFileInfo())
        {
            QFile source(fileObjPlugNetw.absoluteFilePath());
            if(!source.copy(fileObjPlug.absoluteFilePath()))
                return SetError(MAIN_PCOPY_ERR);

            WaitCopyFileDone(fileObjPlug.absoluteFilePath(),fileObjPlugNetw.size());
            source.flush();
            QCoreApplication::processEvents();
            *findedPath = fileObjPlug.absoluteFilePath();
            return 0;
        }
    }

    // check network storage libs dir

    QDir dirLibNetw(m_coreStorageAbsPath + m_libsRelPath);
    if(!dirLibNetw.exists()){
        return SetError(tr("Module %1 not found").arg(name));
    }
    QDir fileDirLibNetw(QString("%1/%2").arg(dirLibNetw.absolutePath()).arg(name));
    if(fileDirLibNetw.exists())
    {
        QFileInfo fileObjLibNetw = FindLastModifFile(fileDirLibNetw.absolutePath());;
        if(fileObjLibNetw!=QFileInfo())
        {
            QFile source(fileObjLibNetw.absoluteFilePath());
            if(!source.copy(fileObjLib.absoluteFilePath()))
                return SetError(MAIN_LCOPY_ERR);

            WaitCopyFileDone(fileObjLib.absoluteFilePath(),fileObjLibNetw.size());
            source.flush();
            QCoreApplication::processEvents();
            *findedPath = fileObjLib.absoluteFilePath();
            return 0;
        }
    }

    return SetError(tr("Module %1 not found").arg(name));
}

QString ModuleFinder::GetPath(Paths path)
{
    QString ret_val = "";
    switch (path) {
    case LIBS_PATH:
        ret_val = m_absAppPath + m_libsRelPath;
        break;
    case PLUG_PATH:
        ret_val = m_absAppPath + m_pluginsRelPath;
        break;
    case CORE_STORAGE_PATH:
        ret_val = m_networkPrefix + m_coreStorageAbsPath;
        break;
    case SCRIPTS_STORAGE_PATH:
        ret_val = m_networkPrefix + m_scriptsStorageAbsPath;
        break;
    case APP_PATH:
        ret_val = m_absAppPath;
        break;
    case BACKUP_PATH:
        ret_val = m_absAppPath + "backups/";
        break;
    default:
        break;
    }
    return ret_val;
}

QFileInfo ModuleFinder::FindLastModifFile(QString path)
{
    QDir dirPath(path);
    QFileInfoList filesList = dirPath.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
    QDateTime lastModifTestFile = QDateTime();
    int latest_index = -1;
    for(int i=0;i<filesList.count();i++)
    {
        QDateTime currFileModif = filesList.at(i).lastModified();
        if(lastModifTestFile < currFileModif){
            lastModifTestFile = currFileModif;
            latest_index = i;
        }
    }
    return (latest_index>-1) ? filesList.at(latest_index) : QFileInfo();
}

void ModuleFinder::WaitCopyFileDone(QString filePath, qint64 size)
{
//    qDebug()<<filePath<<"wait copy process done...";
    QFileInfo finfo(filePath);
    if(finfo.size()==size)
        return;
    QEventLoop loop;
    QTimer::singleShot(500,&loop,SLOT(quit()));
    loop.exec();
    WaitCopyFileDone(filePath,size);
}
