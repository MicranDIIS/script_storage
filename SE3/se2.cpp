#include "se2.h"
#include "ui_se2.h"
#include <QStringList>
#include <QObject>

//#define DEBUG

SE2::SE2(bool isStorageEditable, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SE3)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ui->setupUi(this);

    bReloadScript = false;
    isRunFromConsole_ = false;
    isEditScr = false;
    m_editStorage = isStorageEditable;
    scriptsPath = QApplication::applicationDirPath()+"/../scripts";
//    ui->lwCategories->clear();
    ui->lFileCnt->setText("");
    ui->lFilteredInd->setText(tr("( filtered )"));
    ui->lFilteredInd->setStyleSheet("QLabel { color : red; }");
    ui->lFilteredInd->setVisible(false);

#ifdef DEBUG
    QString baseWinPath = QApplication::applicationDirPath() + QString("/../plugins/BaseWind.dll");
#else
    QString baseWinPath = QApplication::applicationDirPath() + QString("/../plugins/BaseWin.dll");
#endif
    pl_obj = new QPluginLoader(baseWinPath);
    pi_obj = qobject_cast<PluginInterface *>(pl_obj->instance());
    if (!pi_obj){
        QMessageBox::critical(this,tr("Plugin Loader Error"),pl_obj->errorString());
        exit(-1);
    }

//    ui->lwOperations->installEventFilter(this);
//    ui->lwOperations->setFocus();
    connect(this,SIGNAL(reOpenScript(const QModelIndex&)),SLOT(on_lvOperations_doubleClicked(QModelIndex)),Qt::QueuedConnection);
    UpdateLists();

    ui->lvCategories->setModel(scriptManag.proxyModel);
    ui->lvOperations->setModel(scriptManag.proxyModel);

    connect( ui->lvCategories, SIGNAL(clicked(const QModelIndex &)),
                 ui->lvOperations, SLOT(setRootIndex(const QModelIndex &)) );

    connect( ui->lvCategories->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             SLOT(handleSelectionChanged(QItemSelection)));

    QSettings loadState("Micran","SEv3");
    loadState.beginGroup("MainWinState");
    stScriptIndexes currScriptIndexes = scriptManag.GetScriptIndexes(loadState.value("lastScript",0).toString());
    mCurrCategoryRow = currScriptIndexes.categoryIndex;
    mCurrScriptRow = currScriptIndexes.scriptIndex;

    UpdateViewsCurrentIndex();
    loadState.endGroup();
//    ui->lwCategories->hide();
//    ui->lwOperations->hide();
    installEventFilter(this);
    ui->lvOperations->setFocus();
}

SE2::~SE2()
{
    QSettings saveState("Micran","SEv3");
    saveState.beginGroup("MainWinState");
    saveState.setValue("lastScript",ui->lvOperations->currentIndex().data(Qt::ToolTipRole).toString());
    saveState.endGroup();
    pl_obj->unload();
    delete pl_obj;
    delete ui;
}


void SE2::handleSelectionChanged(const QItemSelection& selection){
   if(selection.indexes().isEmpty()) {
//      clearMyView();
   } else {
       ui->lvOperations->setRootIndex(selection.indexes().first());
       ui->lvOperations->setCurrentIndex(ui->lvOperations->model()->index(0,0,selection.indexes().first()));
   }
}

void SE2::ScriptClosed()
{
    qApp->setQuitOnLastWindowClosed(true);
    isEditScr = false;
    if (isRunFromConsole_) {
        this->close();
    }
    else {
        this->show();
        this->raise();
        this->activateWindow();
        if(bReloadScript){
            bReloadScript = false;
            reOpenScript(ui->lvOperations->currentIndex());
        }
    }
}

void SE2::UpdateLists()
{
    int cnt = scriptManag.FindProccedure(scriptsPath);
    UpdateFileCnt(cnt);
    if(ui->lvCategories->model())
        UpdateViewsCurrentIndex();
}

void SE2::RunScriptFromConsole(QString path)
{
    QString specific;
    QString category;
    if (scriptManag.GetScriptHeader(path, specific, category)) {
        LoadScript(path,specific);
        isRunFromConsole_ = true;
    }
    else
        close();
}

bool SE2::LoadScript(QString path, QString header)
{
    Reader::recodeFile(path);
    bool result = false;

    pi_obj = qobject_cast<PluginInterface *>(pl_obj->instance());
    if (!pi_obj){
        QMessageBox::critical(this,tr("PL Error"), pl_obj->errorString());
        return false;
    }
    /// LoadScript(QString[path_to_script ; title ; block_save_changes])
    qDebug()<<"load script"<<path<<header;
    pi_obj->LoadScript(path + ";" + header + ";" + (ui->tbNetwork->isChecked() && !m_editStorage ? "1" : "0"));
    QWidget *pluginForm = pi_obj->GetPtrForm();

    if(pluginForm){
        connect(pluginForm,SIGNAL(ReloadScript()),this,SLOT(ReloadScript()));
        connect(pluginForm,SIGNAL(destroyed()),this,SLOT(ScriptClosed()));

        if(isEditScr)
            pi_obj->setMode(true);
        else
            pluginForm->show();

        pluginForm->setWindowTitle(QString("Script Executor v3 ( %1 )").arg(header));
        pluginForm->setAttribute(Qt::WA_DeleteOnClose);
        qApp->setQuitOnLastWindowClosed(false);
        this->hide();
    }
    return result;
}

bool SE2::eventFilter(QObject *obj, QEvent *e)
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

void SE2::UpdateViewsCurrentIndex()
{
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

void SE2::UpdateFileCnt(int count)
{
    ui->lFileCnt->setText(QString::number(count));
}

void SE2::ReloadScript()
{
    bReloadScript = true;
    reinterpret_cast<QWidget*>(sender())->close();
}

void SE2::on_tbNewScript_clicked()
{
    isEditScr = true;
    LoadScript(qApp->applicationDirPath()+"/../scripts/_pattern.lu","Новый скрипт");
}

void SE2::on_tbEditScript_clicked()
{
    isEditScr = true;
    on_lvOperations_doubleClicked(ui->lvOperations->currentIndex());
}

void SE2::on_tbDelScript_clicked()
{
    int ret = QMessageBox::warning(this, tr("SE2"),
                                   tr("Do you really want to delete this script?"),
                                   QMessageBox::Yes | QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes){
//        QFile file(scriptsList[ui->lwCategories->currentRow()].value(ui->lwOperations->currentItem()->text()));
//        file.remove();
//        UpdateLists();
    }
    else if (ret == QMessageBox::Cancel)
        return;
}

void SE2::on_tbRescan_clicked()
{
    UpdateLists();
}

void SE2::on_tbLaunch_clicked()
{
//    if(mCurrScriptRow)
        on_lvOperations_doubleClicked(ui->lvOperations->currentIndex());
}

void SE2::on_tbNetwork_toggled(bool checked)
{
    ui->lStorage->setText(checked ? tr("Network Storage") : tr("Local Storage"));
    scriptsPath = checked  ? "\\\\k118\\Pribor\\Служебное_ПО\\STORAGE\\scripts" : QApplication::applicationDirPath()+"/../scripts";
    UpdateLists();
}

void SE2::on_tbDir_toggled(bool checked)
{
    ui->gbCategories->setTitle((checked && !ui->tbFind->isChecked()) ? tr(" Directories ") : tr(" Categories "));
    scriptManag.SortFilesUpdateModel(ui->tbFind->isChecked() ? IN_ONE_LIST : checked ? BY_DIR : BY_CATEGORY);
    UpdateViewsCurrentIndex();
}

void SE2::on_tbFind_toggled(bool checked)
{
    ui->gbCategories->setTitle((!checked && ui->tbDir->isChecked()) ? tr(" Directories ") : tr(" Categories "));
    scriptManag.SortFilesUpdateModel(checked ? IN_ONE_LIST : ui->tbDir->isChecked() ? BY_DIR : BY_CATEGORY);
    UpdateViewsCurrentIndex();
}

void SE2::on_leTitleFilter_textChanged(const QString &arg1)
{
    int cnt = scriptManag.FilterResults(arg1);
    UpdateFileCnt(cnt);
    ui->lFilteredInd->setVisible(!arg1.isEmpty() || !ui->leTextFilter->text().isEmpty());
    UpdateViewsCurrentIndex();
}

void SE2::on_leTextFilter_textChanged(const QString &arg1)
{
    scriptManag.proxyModel->setContentFilterText(arg1);
    UpdateFileCnt(scriptManag.RowCount());
    ui->lFilteredInd->setVisible(!arg1.isEmpty() || !ui->leTitleFilter->text().isEmpty());
    UpdateViewsCurrentIndex();
}


void SE2::on_lvOperations_doubleClicked(const QModelIndex &index)
{
    QString path = index.data(Qt::ToolTipRole).toString();
    QString header = index.data(Qt::DisplayRole).toString();
    LoadScript(path, header);
}
