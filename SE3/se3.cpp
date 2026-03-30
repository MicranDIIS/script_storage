/// \file se3.cpp
/// \brief графический интерфейс менеджера скриптов

#include "se3.h"
#include "ui_se3.h"
#include <QStringList>
#include <QSplitter>
#include <QObject>
#include <QFileDialog>

#ifdef WIN32
#include "windows.h"
#endif

//#define DEBUG

SE3::SE3(bool isStorageEditable, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SE3)
{
    qDebug()<<"MAIN THREAD"<<thread();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ui->setupUi(this);

    bReloadScript = false;
    lFlags.isConsole = false;
    lFlags.isEditNow = false;
    lFlags.isEditStorage = isStorageEditable;
    lFlags.isNetworkStorage = false;
    m_editStorage = isStorageEditable;
    scriptManag = new ScriptsManager(parent);
    if(scriptManag->GetLastErrorCode())
    {
        QMessageBox::critical(this,tr("Init ScriptManager error"), scriptManag->GetQSError());
        exit(-1);
        return;
    }

    ui->lFileCnt->setText("");
    ui->lFilteredInd->setText(tr("( filtered )"));
    ui->lFilteredInd->setStyleSheet("QLabel { color : red; }");
    ui->lFilteredInd->setVisible(false);
    ui->tbCopyScript->setEnabled(false);
    ui->tbLinkScript->setEnabled(true);
    ui->tbUpdateScript->setEnabled(false);
    ui->tbRestoreScript->setEnabled(false);


    m_blockArea = new CBlockArea(":/block",16,this);
    m_blockArea->resize(geometry().size());

    connect(this,SIGNAL(reOpenScript(const QModelIndex&)),SLOT(on_lvOperations_doubleClicked(QModelIndex)),Qt::QueuedConnection);
    connect(this,SIGNAL(reOpenScript(QString)),SLOT(RunScriptFromConsole(QString)),Qt::QueuedConnection);

    ui->lvCategories->setModel(scriptManag->model());
    ui->lvOperations->setModel(scriptManag->model());

    connect( ui->lvCategories->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             SLOT(handleSelectionChanged(QItemSelection)));
    connect( ui->lvOperations->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             SLOT(handleSelectionScriptsChanged(QItemSelection)));

    QSettings loadState(scriptManag->GetAppPath() + "/config/commSet.ini",
                       QSettings::IniFormat);

    loadState.setIniCodec("UTF-8");
    loadState.beginGroup("MainWinState");
    lastScriptPath = loadState.value("lastScript","").toString();
    stScriptIndexes currScriptIndexes = scriptManag->GetScriptIndexes(lastScriptPath);
    mCurrCategoryRow = currScriptIndexes.categoryIndex;
    mCurrScriptRow = currScriptIndexes.scriptIndex;
    restoreGeometry(loadState.value("winGeometry",QByteArray()).toByteArray());
    if(loadState.value("splitterState").isValid())
        ui->splitter->restoreState(loadState.value("splitterState",QByteArray()).toByteArray());
    loadState.endGroup();

    installEventFilter(this);
    ui->lvOperations->setFocus();
    ui->lvOperations->setFlow(QListView::TopToBottom);

    scriptItemDelegate *scriptDescrDelegate = new scriptItemDelegate(this);
    ui->lvOperations->setItemDelegate(scriptDescrDelegate);
    scriptItemDelegate *scriptGroupDelegate = new scriptItemDelegate(this);
    ui->lvCategories->setItemDelegate(scriptGroupDelegate);
}

SE3::~SE3()
{
    QSettings saveState(scriptManag->GetAppPath() + "/config/commSet.ini",
                       QSettings::IniFormat);
    saveState.setIniCodec("UTF-8");
    saveState.beginGroup("MainWinState");
    saveState.setValue("lastScript",ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString());
    saveState.setValue("winGeometry",saveGeometry());
    saveState.setValue("splitterState",ui->splitter->saveState());
    saveState.endGroup();


    QSettings localSettings(scriptManag->GetAppPath() + "/config/localSettings.ini",
                            QSettings::IniFormat);
    localSettings.setIniCodec("UTF-8");
    localSettings.setValue("lastLaunchSucc",true);

    delete scriptManag;
    delete ui;
}

void SE3::init()
{
    UpdateLists();
    UpdateViewsCurrentIndex();
}


void SE3::handleSelectionChanged(const QItemSelection& selection)
{
   if(!selection.indexes().isEmpty())
   {
       ui->lvOperations->setRootIndex(selection.indexes().first());
       ui->lvOperations->setCurrentIndex(ui->lvOperations->model()->index(0,0,selection.indexes().first()));
   }
}

void SE3::handleSelectionScriptsChanged(const QItemSelection &selection)
{
    if(!selection.indexes().isEmpty()) {
        CheckRestoreUpdateAvailable();
    }
    else
    {
        ui->tbUpdateScript->setEnabled(false);
        ui->tbRestoreScript->setEnabled(false);
    }
}

void SE3::ScriptClosed()
{
    qDebug()<<"script closed se3 slot";
    qApp->setQuitOnLastWindowClosed(true);
    qApp->processEvents();
    QString tpath = lastScriptPath;
    UpdateLists();
    lastScriptPath = tpath;
    UpdateViewsCurrentIndex();
    if(lFlags.isConsole) {
        if(bReloadScript){
            bReloadScript = false;
            emit reOpenScript(lastScriptPath);
        }
        else
            this->close();
    }
    else
    {
        if(bReloadScript){
            bReloadScript = false;
            emit reOpenScript(ui->lvOperations->currentIndex());
        }
        else{
            this->show();
            this->raise();
            this->activateWindow();
        }
    }
    lFlags.isEditNow = false;
}

void SE3::UpdateLists()
{
    ShowLongMessage(tr("Construct tree of files\n"
                       "    Please wait..."),true);

    SaveCurrentScriptIndex();
    ui->tbNetwork->setEnabled(scriptManag->IsNetworkStorageExists());
    int cnt = scriptManag->FindProccedure();
    UpdateFileCnt(cnt);
    if(ui->lvCategories->model())
        UpdateViewsCurrentIndex();

    ShowLongMessage("",false);
}

void SE3::RunScriptFromConsole(QString path)
{
    QString specific;
    QString category;
    if (scriptManag->GetScriptHeader(path, specific, category)) {
        lFlags.isConsole = true;
        LoadScript(stScriptFileInfo{path,specific,category});
    }
    else
        close();
}

bool SE3::LoadScript(stScriptFileInfo info)
{

    ShowLongMessage(tr("Running the script\n"
                       "    Please wait..."),true);

    if(SetError(scriptManag->LoadScript(info, lFlags))){
        ShowLongMessage(tr(""),false);
        QMessageBox::critical(this,tr("Load Script Error"), scriptManag->GetQSError());
        return false;
    }

    QWidget *pluginForm = scriptManag->GetPtrForm();

    if(pluginForm){
        connect(pluginForm,SIGNAL(ReloadScript(QString)),this,SLOT(ReloadScript(QString)),Qt::QueuedConnection);
        connect(pluginForm,SIGNAL(destroyed()),this,SLOT(ScriptClosed()),Qt::QueuedConnection);

        SaveCurrentScriptIndex();
        pluginForm->setWindowTitle(QString("Script Executor v3 ( %1 )").arg(info.header));
        pluginForm->setAttribute(Qt::WA_DeleteOnClose);
        pluginForm->show();
        qApp->setQuitOnLastWindowClosed(false);
        this->hide();
    }

    ShowLongMessage(tr(""),false);
    return true;
}



bool SE3::eventFilter(QObject *obj, QEvent *e)
{
    //Отлавливаем событие нажатия клавиши Enter
    if(e->type() == QEvent::KeyPress){
        QKeyEvent * ke = static_cast<QKeyEvent*>(e);
        if(ke->key()== Qt::Key_Enter || ke->key()== Qt::Key_Return){
            on_lvOperations_doubleClicked(ui->lvOperations->currentIndex());
        }
    }
    return  QWidget::eventFilter(obj, e);//Вызываем стандартный обработчик событий
}

QString SE3::GetCurrScriptHead()
{
    return ui->lvOperations->currentIndex().data(Qt::DisplayRole).toString();
}

QString SE3::GetCurrScriptPath()
{
    return ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString();
}

void SE3::UpdateViewsCurrentIndex()
{
    RestoreCurrentScriptIndex();
    int rowCountCat = ui->lvCategories->model()->rowCount();
    if(rowCountCat){
        if(rowCountCat<mCurrCategoryRow+1)
            mCurrCategoryRow = rowCountCat-1;
        ui->lvCategories->setCurrentIndex(ui->lvCategories->model()->index(mCurrCategoryRow,0));
        ui->lvOperations->setRootIndex(ui->lvCategories->model()->index(mCurrCategoryRow,0));
        const QModelIndex rootIndex = ui->lvOperations->rootIndex();

        int rowCountScr = ui->lvOperations->model()->rowCount(rootIndex);

        if(rowCountScr){
            if(rowCountScr<mCurrScriptRow+1)
                mCurrScriptRow = rowCountScr-1;
            ui->lvOperations->setCurrentIndex(ui->lvOperations->model()->index(mCurrScriptRow,0,rootIndex));
        }
    }
}

void SE3::UpdateFileCnt(int count)
{
    ui->lFileCnt->setText(QString::number(count));
}

void SE3::ShowLongMessage(QString text, bool show)
{
    ui->lvCategories->setEnabled(!show);
    ui->lvOperations->setEnabled(!show);

    m_blockArea->SetText(text);
    if(show)
        m_blockArea->ShowArea();
    else
        m_blockArea->HideArea();
}

void SE3::SaveCurrentScriptIndex()
{
    if(ui->lvOperations->currentIndex().isValid())
    {
        QString path = ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString();
        if(!path.isEmpty())
            this->lastScriptPath = path;
    }
}

void SE3::RestoreCurrentScriptIndex()
{
    if(ui->lvCategories->model()->rowCount())
    {

        stScriptIndexes currScriptIndexes = scriptManag->GetScriptIndexes(lastScriptPath);
        qDebug()<<"restore script indexes"<<lastScriptPath<<currScriptIndexes.categoryIndex<<currScriptIndexes.scriptIndex;
        mCurrCategoryRow = currScriptIndexes.categoryIndex==-1 ?
                    (mCurrCategoryRow>=0 ? mCurrCategoryRow : 0) :
                    currScriptIndexes.categoryIndex;
        mCurrScriptRow = currScriptIndexes.scriptIndex==-1 ?
                    (mCurrScriptRow>=0 ? mCurrScriptRow : 0)  :
                    currScriptIndexes.scriptIndex;
    }
}

void SE3::CheckRestoreUpdateAvailable()
{
    QModelIndex currMIndex = ui->lvOperations->currentIndex();
    QString fullCurrScriptPath = currMIndex.data(Qt::ToolTipRole).toString();
    ui->tbUpdateScript->setEnabled(scriptManag->IsUpdateAvailable(fullCurrScriptPath));
    ui->tbRestoreScript->setEnabled(scriptManag->IsBackUpAvailable(fullCurrScriptPath));
}

void SE3::ReloadScript(QString path)
{
    bReloadScript = true;
    this->lastScriptPath = path;
    if(sender()!=this)
        reinterpret_cast<QWidget*>(sender())->close();
}

void SE3::on_tbNewScript_clicked()
{
    MetaDialog dialog;
    dialog.init(ui->lvCategories->currentIndex().data().toString());
    connect(&dialog,SIGNAL(updateParams(QString,QString,int)),this,SLOT(SaveNewFile(QString,QString,int)),Qt::QueuedConnection);
    connect(&dialog,SIGNAL(done()),&dialog,SLOT(close()));
    dialog.exec();
}

void SE3::on_tbEditScript_clicked()
{
    lFlags.isEditNow = true;
    on_lvOperations_doubleClicked(ui->lvOperations->currentIndex());
}

void SE3::on_tbDelScript_clicked()
{
    int ret = QMessageBox::warning(this, tr("SE3"),
                                   tr("Do you really want to delete this script?"),
                                   QMessageBox::Yes | QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes){
        QString dpath = ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString();;
        QFile::remove(dpath);
        UpdateLists();
    }
    else if (ret == QMessageBox::Cancel)
        return;
}

void SE3::on_tbRescan_clicked()
{
    UpdateLists();
}

void SE3::on_tbLaunch_clicked()
{
    on_lvOperations_doubleClicked(ui->lvOperations->currentIndex());
}

void SE3::on_tbNetwork_toggled(bool checked)
{
    ui->lStorage->setText(checked ? tr("Network Storage") : tr("Local Storage"));
    ui->tbNewScript->setEnabled(!checked);
    ui->tbDelScript->setEnabled(!checked);
    ui->tbCopyScript->setEnabled(checked);
    ui->tbLinkScript->setEnabled(!checked);

    lFlags.isNetworkStorage = checked;
    scriptManag->SwitchToNetworkStorage(checked);
    UpdateLists();
}

void SE3::on_tbDir_toggled(bool checked)
{
    ShowLongMessage(tr("Construct tree of files\n"
                       "    Please wait..."),true);
    SaveCurrentScriptIndex();
    ui->gbCategories->setTitle((checked && !ui->tbFind->isChecked()) ? tr(" Directories ") : tr(" Categories "));
    scriptManag->SortFilesUpdateModel(ui->tbFind->isChecked() ? IN_ONE_LIST : checked ? BY_DIR : BY_CATEGORY);
    UpdateViewsCurrentIndex();
    ShowLongMessage("",false);
}

void SE3::on_tbFind_toggled(bool checked)
{
    ShowLongMessage(tr("Construct tree of files\n"
                       "    Please wait..."),true);
    SaveCurrentScriptIndex();
    ui->gbCategories->setTitle((!checked && ui->tbDir->isChecked()) ? tr(" Directories ") : tr(" Categories "));
    scriptManag->SortFilesUpdateModel(checked ? IN_ONE_LIST : ui->tbDir->isChecked() ? BY_DIR : BY_CATEGORY);
    UpdateViewsCurrentIndex();
    ShowLongMessage("",false);
}

void SE3::on_leTitleFilter_textChanged(const QString &arg1)
{
    int cnt = scriptManag->FilterResults(arg1);
    UpdateFileCnt(cnt);
    ui->lFilteredInd->setVisible(!arg1.isEmpty() || !ui->leTextFilter->text().isEmpty());
    UpdateViewsCurrentIndex();
}

void SE3::on_leTextFilter_textChanged(const QString &arg1)
{
    scriptManag->model()->setContentFilterText(arg1);
    UpdateFileCnt(scriptManag->RowCount());
    ui->lFilteredInd->setVisible(!arg1.isEmpty() || !ui->leTitleFilter->text().isEmpty());
    UpdateViewsCurrentIndex();
}

void SE3::on_lvOperations_doubleClicked(const QModelIndex &index)
{
    QString path = index.data(Qt::ToolTipRole).toString();
    QString header = index.data(Qt::DisplayRole).toString();
    LoadScript(stScriptFileInfo{path, header,""});
}

void SE3::on_tbCopyScript_clicked()
{
    QString sourceFile = ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString();
    if(!scriptManag->CopyFromNetwork(sourceFile))
        QMessageBox::critical(this,tr("Error"),tr("File copying error\n%1").arg(scriptManag->GetQSError()));
    else
        QMessageBox::information(this,tr("Information"),tr("File copied successfully"));
}

void SE3::on_tbLinkScript_clicked()
{
    if(scriptManag->CreateLinkToScript(GetCurrScriptPath(),GetCurrScriptHead()))
        QMessageBox::critical(this,tr("Error"),tr("Failed to create link"));
    else
        QMessageBox::information(this,tr("Information"),tr("Desktop link created successfully "));
}

void SE3::on_tbUpdateScript_clicked()
{
    QString scriptPath = GetCurrScriptPath();
    QString resMessage = tr("Backup is success");

    if(!scriptManag->BackupScript(scriptPath))
        resMessage = tr("Backup is failed");

    if(!scriptManag->UpdateScript(scriptPath))
    {
        QMessageBox::information(this,tr("Information"),tr("The script was updated successfully\n %1").arg(resMessage));
        UpdateLists();
    }
    else
        QMessageBox::critical(this,tr("Error"),tr("%1\n %2").arg(scriptManag->GetQSError()).arg(resMessage));

    CheckRestoreUpdateAvailable();
}

void SE3::on_tbRestoreScript_clicked()
{
    if(!scriptManag->RestoreScript(GetCurrScriptPath()))
        QMessageBox::critical(this,tr("Failed to restore script"),scriptManag->GetQSError());
    else
    {
        QMessageBox::information(this,tr("Information"),tr("The script was successfully restored"));
        UpdateLists();
    }
    CheckRestoreUpdateAvailable();
}

void SE3::SaveNewFile(QString title, QString category, int lang)
{
    QString extDescr = lang ? tr("Python-script (*.py)") : tr("Lua-script (*.lua)");
    QString path =  QFileDialog::getSaveFileName(this, tr("Save File As"),
                                                     qApp->applicationDirPath()+QString("\\..\\scripts\\%1").arg(title),
                                                     extDescr);
    if (!path.isEmpty())
    {
            if(scriptManag->CreateNewScript(stScriptFileInfo{path,title,category},lang))
            {
                QMessageBox::critical(this,tr("Failed to create new script"),scriptManag->GetQSError());
                return;
            }


            UpdateLists();
            lastScriptPath = path;
            UpdateViewsCurrentIndex();

            qApp->processEvents();
            emit reOpenScript(ui->lvOperations->currentIndex());
    }
}
