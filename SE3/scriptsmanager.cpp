/// \file scriptsmanager.cpp
/// \brief основные рабочие функции менеджера скриптов

#include "scriptsmanager.h"
#include "include/createlink.h"
#include <QString>

#ifndef Q_OS_WIN
#include "../include/mount_netw_folder.h"
#endif

ScriptsManager::ScriptsManager(QWidget* parent): QObject(parent)
{
    qDebug()<<"scr mng construct";

    qDebug()<<"create finder";
    mp_finder = new ModuleFinder();
    m_app_path = mp_finder->GetPath(APP_PATH);

    QSettings localSettings(GetAppPath() + "/config/localSettings.ini",
                            QSettings::IniFormat);
    localSettings.setIniCodec("UTF-8");
    m_scriptExclDir = localSettings.value("scriptExcludeDir","").toString().split(";");

    qDebug()<<"create updater";
    mpUpdater = new Updater(m_app_path);
    qDebug()<<"updater construct ok";
    if(IsNetworkStorageExists()){
        if(!mpUpdater->ParseJSONFile(mp_finder->GetPath(SCRIPTS_STORAGE_PATH)+"updates.json"))
            mpUpdater->CheckForUpdate();
    }
    else
        qDebug()<<mpUpdater->GetQSError();

    m_backup_script_path = mp_finder->GetPath(BACKUP_PATH)+TITLE_BKUP_SCRIPTS;
    mpBackuper = new BackUper(m_app_path,m_backup_script_path);
    mpBackuper->InitArchive(m_backup_script_path);
    mpBackuper->GetQSError(); //clear error

    mSortMode = BY_CATEGORY;
    treemodel = new TreeModel(mpUpdater, mpBackuper);
    proxyModel = new NameContentFilterModel(nullptr);
    proxyModel->setSourceModel(treemodel);

    SwitchToNetworkStorage(false);

    QString baseWinPath;
    QString baseWinName;
#ifdef DEBUG
    baseWinName = "BaseWinPyd.dll";
#else
    #ifdef WIN32
        baseWinName = "BaseWinPy.dll";
    #else
        baseWinName = "libBaseWinPy.so";
    #endif
#endif
	pl_obj = 0;
//    int find_res = mp_finder->FindModule(baseWinName,&baseWinPath);
//    if(find_res!=SE3_NO_ERROR)
//    {
//        if(find_res==USER_ERROR)
//            SetError(mp_finder->GetQSError());
//        else
//            SetError(find_res);
//        return;
//    }

//    pl_obj = new QPluginLoader(baseWinPath);
//    mPiObj = qobject_cast<PluginInterface *>(pl_obj->instance());
//    if(!mPiObj){
//        SetError(pl_obj->errorString());
//        return;
//    }


}

ScriptsManager::~ScriptsManager()
{
    mp_finder->deleteLater();
    if(pl_obj)
    {
        pl_obj->unload();
        delete pl_obj;
    }
    delete treemodel;
    delete proxyModel;
    delete mpUpdater;
    delete mpBackuper;
}

int ScriptsManager::FindProccedure()
{
    mpUpdater->CheckForUpdate();
    mpBackuper->InitArchive(m_backup_script_path);
    mpBackuper->GetQSError();
    mScriptInfo.clear();
    mFindedFiles = FindFiles(mScriptPath,m_netwStorage ? QStringList() : QStringList()<<"*.py"<<"*.lua");
    return SortFilesUpdateModel(mSortMode);
}

QStringList ScriptsManager::FindFiles(QString path, QStringList filter)
{
    QStringList findedFiles;
    if(!path.isEmpty()){
        QDir d(path);
        d.setNameFilters(filter);
        QFileInfoList infoList = d.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
        for (int i = 0; i < infoList.count(); i ++){
            if(infoList.at(i).isDir()){
                if(infoList.at(i).path().contains(QDir::toNativeSeparators("scripts/modules")))
                    break;
                findedFiles.append(FindFiles(infoList.at(i).absoluteFilePath(),filter));
                continue;
            }
            findedFiles.append(infoList.at(i).absoluteFilePath());
        }
    }
    return findedFiles;
}

QList<stScriptInfo> ScriptsManager::ExtractScriptsInfo(QStringList files)
{
    QList<stScriptInfo> infoList;
    foreach (QString filePath, files) {
        QString title = "";
        QString category = "";
        if (GetScriptHeader(filePath, title, category)){
            int index = 0;
            if(GetCategoryIndex(infoList,category,index)){
                infoList[index].script.insert(title,filePath);
            }
            else{
                QMap<QString,QString>temp;
                temp[title]=filePath;
                infoList.append(stScriptInfo{category,temp});
            }
        }
//        Reader::recodeFile(filePath);
    }
    return infoList;
}

QList<stScriptInfo> ScriptsManager::CorrectTitles(QList<stScriptInfo> list)
{
    QList<stScriptInfo> correctedList;
    foreach (stScriptInfo element, list) {
        QStringList titleList = element.script.keys();
        QMap<QString,QString> correctedTitleList;
        for (int i=0;i<titleList.size();i++)
        {
            QString tmp_name = titleList[i];
            if(correctedTitleList.contains(tmp_name)){
                for(int j=1;j<10;j++)
                {
                    tmp_name = titleList[i] + " ( copy " + QString::number(j) + " )";
                    if(!correctedTitleList.contains(tmp_name))
                        break;
                }
            }
            correctedTitleList.insert(tmp_name,element.script.value(titleList[i]));
        }
        element.script = correctedTitleList;
        correctedList.append(element);
    }
    return correctedList;
}

bool ScriptsManager::GetScriptHeader(const QString path, QString &title, QString &category)
{
    bool result = false;
    category = "";
    title = "";
    Reader::recodeFile(path);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) // пробуем открыть файл
    {
        QTextStream in(&file); // добавляем файл в текстовый поток
        in.setCodec("utf-8");
        QString line;
        while(!in.atEnd())
            line = in.readLine();

        // search new script header
        if( (result = Reader::checkLine(line)) ){
            stDecodeData data = Reader::unpackLine(line);
            title = data.title;
            category = data.category;

        }
        else{// search old script header
            in.seek(0);
            line = in.readLine();
            if( (result = line.contains("--Specific:") || line.contains("##Specific:")) ){ // если есть строка --Specific: - то это наш клиент
                int start = line.indexOf("Specific:")+9;
                title = line.mid(start,line.indexOf(";")-start).trimmed(); //Title

                if(line.contains("Categories:")){
                    start = line.indexOf("Categories:")+11;
                    category = line.mid(start,line.lastIndexOf(";")-start).trimmed(); //Categories
                }
            }
            else { // clear script
                if(!m_scriptExclDir.contains(QFileInfo(path).dir().dirName()))
                {
                    result = true;
                    title = m_netwStorage ? QFileInfo(path).dir().dirName() : QFileInfo(path).baseName();
                    category = tr("Clear scripts");
                }
            }
        }

        if(result){
            switch (mSortMode) {
            case BY_DIR:
                category=QFileInfo(path).dir().dirName();
                break;
            case BY_CATEGORY:
                if(category.isEmpty())
                    category=QObject::tr("Other");
                break;
            case IN_ONE_LIST:
                category=QObject::tr("All Scripts");
                break;
            }
        }
    }
    return result;
}

stScriptIndexes ScriptsManager::GetScriptIndexes(QString path)
{
    for(int i=0; i<mScriptInfo.size();i++)
    {
        int ind = mScriptInfo[i].script.values().indexOf(path);
        if(ind>=0)
            return stScriptIndexes{i,ind};
    }
    return stScriptIndexes{-1,-1};
}

bool ScriptsManager::GetCategoryIndex(QList<stScriptInfo> list, QString category, int &index)
{
    for (int i=0;i<list.size();i++) {
        if(list[i].category == category){
            index = i;
            return true;
        }
    }
    return false;
}

QString ScriptsManager::getRelPath(QString fullPath)
{
    QFileInfo fileiInfo(m_app_path);
    QDir dir = fileiInfo.dir();
    return dir.relativeFilePath(fullPath);
}

void ScriptsManager::clean()
{
    if(pl_obj)
    {
        qDebug()<<"plugin"<<pl_obj;
//        qDebug()<<"plugin unload"<<pl_obj->unload();
        pl_obj->deleteLater();
        pl_obj = NULL;
    }
    qApp->processEvents();
}



int ScriptsManager::RowCount(const QModelIndex & index)
{
    int count = 0;
    int rowCount = proxyModel->rowCount(index);
    if(index!=QModelIndex())
        count += rowCount;

    for( int r = 0; r < rowCount; ++r )
        count += RowCount( proxyModel->index(r,0,index) );

    return count;
}

int ScriptsManager::UpdateScript(const QString &path)
{
    if(mpUpdater->UpdateFile(path))
        return SetError(mpUpdater->GetQSError());
    return SE3_NO_ERROR;
}

bool ScriptsManager::BackupScript(const QString &path)
{
    QFileInfo testBacUp(m_backup_script_path);
    QDir dir = testBacUp.dir();
    if (! dir.exists()) {
        dir.mkpath(dir.path());
    }

    int resBckUp = SE3_NO_ERROR;
    if(!dir.exists())
        resBckUp = SetError(BCK_CREATE_DIR_ERR);

    QString relPath = getRelPath(path);

//    if(testBacUp.exists())
//    {
//        resBckUp = mpBackuper->AppendFileToArchive(testBacUp.absoluteFilePath(),relPath);
//    }
//    else
//    {
//        resBckUp = mpBackuper->AddFile(relPath);
//        resBckUp += mpBackuper->CreateBackUp(testBacUp.absoluteFilePath());
//    }
    resBckUp = mpBackuper->BackUp(relPath);
    qDebug()<<mpBackuper->GetQSError()<<"rel_path:"<<relPath<<"path:"<<path;
    return resBckUp == SE3_NO_ERROR;
}

bool ScriptsManager::RestoreScript(const QString &path)
{
    QFileInfo testBackUp(m_backup_script_path);
    QString relPath = getRelPath(path);
    if(SetError(mpBackuper->RestoreFileFromArchive(testBackUp.absoluteFilePath(),relPath)))
        return false;
    return true;
}

bool ScriptsManager::IsUpdateAvailable(const QString &path)
{
    return mpUpdater->IsUpdateAvailable(path);
}

bool ScriptsManager::IsBackUpAvailable(const QString &path)
{
    return mpBackuper->CheckFileInArchive(path);
}

bool ScriptsManager::CreateLinkToScript(const QString &path, const QString &header)
{
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    return true;/*CreateLink((const wchar_t*)QString(qApp->applicationDirPath()+"/SE3.exe").utf16(),
                      (const wchar_t*)path.utf16(),
                      codec->fromUnicode(QString("%1/%2.lnk").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).arg(header)).toStdString().c_str(),
                      (const wchar_t*)QString(qApp->applicationDirPath()+"/../config/ico.ico").utf16());*/

}

bool ScriptsManager::CopyFromNetwork(const QString &sourecePath,const QString &suffix,QString *tFileName)
{
    int lastInd = sourecePath.lastIndexOf("/");
    int preLastInd = sourecePath.lastIndexOf("/",lastInd-1)+1;
    QString targetFileName = sourecePath.mid(preLastInd,lastInd-preLastInd);
    int extCnt = targetFileName.length()-targetFileName.lastIndexOf(".");
    QString tagetExt = targetFileName.right(extCnt);
    targetFileName.chop(extCnt);

    QString targetDir = GetAppPath()+"scripts";
    if(!QDir(targetDir).exists())
    {
        QDir dir;
        if(!dir.mkpath(targetDir))
        {
            SetError(tr("Cannot create target directory"));
            return false;
        }
    }


    QString targetFile = QString("%1scripts/%2%3%4").arg(GetAppPath()).arg(targetFileName).arg(suffix).arg(tagetExt);
    if(tFileName)
        *tFileName = targetFile;
    QFile sFile(sourecePath);
    if(!sFile.copy(targetFile))
    {
        SetError(sFile.errorString());
        return false;
    }
    return true;
}

void ScriptsManager::SwitchToNetworkStorage(bool on)
{
    m_netwStorage = on;
    mScriptPath = on  ? mp_finder->GetPath(SCRIPTS_STORAGE_PATH) + "scripts/"
                      : mp_finder->GetPath(APP_PATH) + "scripts/";
}

bool ScriptsManager::IsNetworkStorageExists()
{
    bool exist = QDir(mp_finder->GetPath(SCRIPTS_STORAGE_PATH)).exists();
#ifdef Q_OS_UNIX
    if(!exist){
        QSettings localSettings(GetAppPath() + "/config/localSettings.ini",
                                QSettings::IniFormat);
        localSettings.setIniCodec("UTF-8");
        QString mPoint = localSettings.value("mountPointName","").toString();
        if (!mPoint.isEmpty())
            mPoint = "/mnt/"+mPoint;
        if(!MountNetworkFolder((QWidget*)parent(),mp_finder,mPoint))
            exist = QDir(mp_finder->GetPath(SCRIPTS_STORAGE_PATH)).exists();
    }

#endif
    return exist;
}

int ScriptsManager::LoadScript(stScriptFileInfo info, stLoadFlags lFlags)
{

    if(info.path.isEmpty() || info.header.isEmpty() || !QFile::exists(info.path))
        return SetError(tr("Incorrect input params"));

    Reader::recodeFile(info.path); /// исправление кодировки

//    QString baseWinPath;
//    mp_finder->FindModule("BaseWinPy.dll",&baseWinPath);
//    pl_obj = new QPluginLoader(baseWinPath);

    mPiObj = qobject_cast<PluginInterface *>(pl_obj->instance());
    if (!mPiObj){
        return SetError(pl_obj->errorString());
    }
    if(lFlags.isEditNow)
        mPiObj->setMode(EDIT_NOW);
    mPiObj->LoadScript(info.path);
    if(mPiObj->GetPtrForm())
    {
//        connect(mPiObj->GetPtrForm(),SIGNAL(destroyed()),this,SLOT(clean()));
        if(lFlags.isNetworkStorage)
        {
            if(lFlags.isEditStorage)
                mPiObj->setMode(EDIT_NETWORK_STORAGE);
            else
                mPiObj->setMode(NETWORK_STORAGE);
        }
        else
            if(lFlags.isConsole)
                mPiObj->setMode(RUN_FROM_CONSOLE);
        return SE3_NO_ERROR;
    }

    return SetError(tr("Unable load script"));
}

int ScriptsManager::CreateNewScript(stScriptFileInfo info, int lang)
{
    QString script_text_lua = "local base = require(\"base\")\n   -- Подключаем стандартную библиотеку служебных функций \
\n\
--! Заполняем массив необходимых модулей (первый модуль в списке занимает центральный виджет)\n\
LModList = {\"Log.dll\",\"logWin\",\n\
            \"Request.dll\",\"inputData\"}\n\
            \n\
base.RegModule(LModList)  -- Регистрируем выбранные модули\n\
--! Формируем необходимые нам запросы\n\
--[[	inputData:addRequest(\"Режим проснятия:\",1,\"meas_mode\",\"одиночный|циклический\")\n\
--]]\n\
\n\
--! Формируем основную рабочую функцию\n\
function Run()\n\
        inputData:RequestValues()           -- Запрашиваем введённые в запросы данные\n\
        logWin:Clear()                      -- Чистим лог\n\
        logWin:addRecord(\"Hello world\")   -- Выводим в лог тестовое сообщение\n\
end\n\
\n\
--! При ошибке и прерывании вызываем функцию\n\
function FinallyExec()\n\
end";

    QString script_text_py = "import mibase\n # Подключаем стандартную библиотеку служебных функций \
\n\
# Заполняем массив необходимых модулей (первый модуль в списке занимает центральный виджет)\n\
LModList = [\"Log.dll\",\"logWin\",\n\
            \"Request.dll\",\"inputData\"]\n\
            \n\
mibase.RegMod(LModList)  # Регистрируем выбранные модули\n\
logWin = mibase.logWin \n\
inputData = mibase.inputData \n\
# Формируем необходимые нам запросы\n\
#	inputData.addRequest(\"Режим проснятия:\",1,\"meas_mode\",\"одиночный|циклический\")\n\
#\n\
\n\
# Формируем основную рабочую функцию\n\
def Run():\n\
    global inputData,logWin\n\
    logWin.Clear()                    # Чистим лог\n\
    logWin.addRecord(\"Hello world\") # Выводим в лог тестовое сообщение\n\
    inputData.RequestValues()         # Запрашиваем введённые в запросы данные\n\
\n\
# При ошибке и прерывании вызываем функцию\n\
def FinallyExec():\n\
    null";

    stDecodeData metaData;
    metaData.title = info.header;
    metaData.category = info.category;
    if(!Reader::packMetaInFile(info.path,metaData,lang ? script_text_py : script_text_lua))
        return SetError(tr("Unable create new file: incorrect path"));
    return SE3_NO_ERROR;
}

bool compareScriptInfo(const stScriptInfo &a, const stScriptInfo &b) {
    return a.category.toLower() < b.category.toLower();
}

int ScriptsManager::SortFilesUpdateModel(SortMode mode)
{
    mSortMode = mode;
    QList<stScriptInfo> infoList = ExtractScriptsInfo(mFindedFiles);
    mScriptInfo = CorrectTitles(infoList);

    qSort(mScriptInfo.begin(),mScriptInfo.end(),compareScriptInfo);
    treemodel->updateData(mScriptInfo);
    return RowCount();
}

int ScriptsManager::FilterResults(QString filter)
{
    proxyModel->setFilterWildcard(filter);
    return RowCount();
}
