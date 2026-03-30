#include "relpathfinder.h"
#include <QDir>

RelPathFinder::RelPathFinder(const QString &baseFileName, const QStringList &fileList, const QString &rootShift, int deepScanV, int deepScanH)
    : m_appfile(baseFileName),m_fileList(fileList),m_relroot(rootShift),m_vDeep(deepScanV),m_hDeep(deepScanH)
{

}

void RelPathFinder::run()
{
    QStringList resultList;
    foreach (const QString& curr_file, m_fileList) {
        QString rel_path = FullToRelPath(curr_file);
//        if(!rel_path.isEmpty())
            resultList.append(rel_path);
    }
    emit resultReady(resultList);
}

QString RelPathFinder::FullToRelPath(const QString &fullPath)
{
    QString appPath;
    QDir currDir(fullPath);
    for(int i = 0; i < m_hDeep; i++){
        if(FindAppDir(currDir.absolutePath(),appPath,0)){
            break;
        }
        if(!currDir.cdUp()){
            break;
        }
    }
    if(appPath.isEmpty())
        return "";
    QFileInfo fileInfo(fullPath);
    QDir ap(QDir::cleanPath(appPath+"/"+m_relroot));
    return ap.relativeFilePath(fullPath).remove("/"+fileInfo.fileName());
}

bool RelPathFinder::FindAppDir(const QString &path, QString &resPath, int currDeep)
{
    if(currDeep>m_vDeep)
        return false;

    if(!path.isEmpty()){
        if(QFileInfo::exists(QString("%1/%2").arg(path).arg(m_appfile))){
            resPath = path;
            return true;
        }

        QDir d(path);
        QFileInfoList infoList = d.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot);
        currDeep++;
        for (int i = 0; i < infoList.count(); i++){
            if(FindAppDir(infoList.at(i).absoluteFilePath(),resPath,currDeep))
                return true;
        }
    }
    return false;
}
