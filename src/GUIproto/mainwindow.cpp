#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "scriptloader.h"
#include "diffviewerwindow.h"
#include "historywindow.h"
#include "scriptwindow.h"

#include <mgit.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "scriptwindow.h"

#include <QCoreApplication>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    basicScriptsModel(new ViewModel(this)),
    customScriptsModel(new ViewModel(this)),

    basicFilterModel(new ScriptFilterModel(this)),
    customFilterModel(new ScriptFilterModel(this)),

    deviceComboModel(new ComboFilterModel(this)),
    roleComboModel(new ComboFilterModel(this)),
    stageComboModel(new ComboFilterModel(this)),
    categoryComboModel(new ComboFilterModel(this)),

    m_repo(0)
{
    ui->setupUi(this);

    setupModels();
    setWindowTitle("SE2");

    setupPageConnect();

    if(!syncRepo()){
        QMessageBox::critical(this, tr("Repository error"), tr("Repository synchronization failed."));
    };

    loadScripts();

    setupFilterConnect();
    setupActionConnect();
}

MainWindow::~MainWindow()
{
    if (m_repo)
    {
        deleteRepository(m_repo);
        m_repo = 0;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupActionConnect(){

    connect(ui->listViewBasic, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(handleScriptDoubleClick(QModelIndex)));
    connect(ui->pushButton, SIGNAL(clicked()),this, SLOT(openSelectedScript()));

    ui->listViewCustom->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listViewCustom, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(showCustomContextMenu(const QPoint &)));

    ui->listViewBasic->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listViewBasic, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(showBasicContextMenu(const QPoint &)));
}

void MainWindow::setupFilterConnect(){

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(applyTextSearch(QString)));
    connect(ui->dComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(applyDeviceFilter()));
    connect(ui->rComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyRoleFilter()));
    connect(ui->sComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyStageFilter()));
}

void MainWindow::setupPageConnect(){

    ui->radioBasic->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);

    connect(ui->radioBasic, SIGNAL(clicked()), this, SLOT(showBasicPage()));
    connect(ui->radioCustom, SIGNAL(clicked()), this, SLOT(showCustomPage()));
    connect(ui->categoryComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyCategoryFilter()));
}

void MainWindow::setupModels(){

    basicScriptsModel->setViewMode(ViewModel::basicMode);
    customScriptsModel->setViewMode(ViewModel::customMode);

    basicFilterModel->setSourceModel(basicScriptsModel);
    basicFilterModel->setMode(ViewModel::basicMode);

    customFilterModel->setSourceModel(customScriptsModel);
    customFilterModel->setMode(ViewModel::customMode);

    ui->listViewBasic->setModel(basicFilterModel);
    ui->listViewBasic->setModelColumn(ViewModel::DisplayColumn);

    ui->listViewCustom->setModel(customFilterModel);
    ui->listViewCustom->setModelColumn(ViewModel::DisplayColumn);

    deviceComboModel->setSourceModel(basicScriptsModel);
    deviceComboModel->setTargetColumn(ViewModel::DeviceColumn);
    ui->dComboBox->setModel(deviceComboModel);

    roleComboModel->setSourceModel(basicScriptsModel);
    roleComboModel->setTargetColumn(ViewModel::RoleColumn);
    ui->rComboBox->setModel(roleComboModel);

    stageComboModel->setSourceModel(basicScriptsModel);
    stageComboModel->setTargetColumn(ViewModel::StageColumn);
    ui->sComboBox->setModel(stageComboModel);

    categoryComboModel->setSourceModel(customScriptsModel);
    categoryComboModel->setTargetColumn(ViewModel::CategoryColumn);
    ui->categoryComboBox->setModel(categoryComboModel);
}

void MainWindow::saveSettings(){

    settings.beginGroup("mainwindow");
    settings.setValue("filters/device", ui->dComboBox->currentText());
    settings.setValue("filters/role",ui->rComboBox->itemData(ui->rComboBox->currentIndex(),Qt::UserRole).toString());
    settings.setValue("filters/stage",ui->sComboBox->currentText());
    settings.setValue("filters/category",ui->categoryComboBox->currentText());
    settings.setValue("win/geometry", saveGeometry());
    settings.setValue("win/state", saveState());
    settings.endGroup();
}

void MainWindow::loadState(){

    settings.beginGroup("mainwindow");
    QString resDevice = settings.value("filters/device").toString();
    int deviceIndex = ui->dComboBox->findText(resDevice);
    if (deviceIndex >= 0) {
        ui->dComboBox->setCurrentIndex(deviceIndex);
    }
    applyDeviceFilter();

    QString resRole = settings.value("filters/role").toString();
    int roleIndex = ui->rComboBox->findData(resRole, Qt::UserRole);
    if (roleIndex >= 0) {
        ui->rComboBox->setCurrentIndex(roleIndex);
    }
    applyRoleFilter();

    QString resStage = settings.value("filters/stage").toString();
    int stageIndex = ui->sComboBox->findText(resStage);
    if (stageIndex >= 0) {
        ui->sComboBox->setCurrentIndex(stageIndex);
    }
    applyStageFilter();

    QString resCategory = settings.value("filters/category").toString();
    int categoryIndex = ui->categoryComboBox->findText(resCategory);
    if (categoryIndex >= 0) {
        ui->categoryComboBox->setCurrentIndex(categoryIndex);
    }
    applyCategoryFilter();

    restoreGeometry(settings.value("win/geometry").toByteArray());
    settings.endGroup();
}

bool MainWindow::syncRepo(){

    if (m_repo)
    {
        deleteRepository(m_repo);
        m_repo = 0;
    }

    QString repoConfigPath = QDir(QString(CONFIG_DIR)).absoluteFilePath("repo.ini");

    IniSettingReader reader;
    RepoConfig repoConfig;

    if (!reader.loadRepo(repoConfigPath, repoConfig)) {
        if((repoConfig.path.isEmpty()||repoConfig.username.isEmpty()||repoConfig.token.isEmpty())&& !repoConfig.url.isEmpty()){
            QMessageBox::warning(this,tr("Remote config error"), tr("Check the config settings for correct field filling:\n")+ repoConfigPath);
            return false;
        } else{
            QMessageBox::critical(this,tr("Remote config error"), tr("Configuration file for remote could not be loaded:\n") + repoConfigPath);
            return false;
        }
    }

    m_repoRoot = repoConfig.path;

    QDir repoDir(repoConfig.path);
    QDir gitDir(repoDir.absoluteFilePath(".git"));

    if (repoDir.exists() && !gitDir.exists()) {
        QStringList entries = repoDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        if (!entries.isEmpty()) {
            QMessageBox::critical(this, tr("Repository error"),tr("Folder for remote repositiry is not empty."));
            return false;
        }
    }

    m_repo = createRepository(repoConfig);
    if (!m_repo)
    {
        QMessageBox::critical(this, tr("Repository error"),
                             tr("Repository object could not be created."));
        return false;
    }

    GitError err;

    if (!gitDir.exists()) {
        err = m_repo->clone();
        if (!err.success) {
            QMessageBox::critical(this, tr("Repository error"),
                                 tr("Repository clone failed:\n") + err.message);
            deleteRepository(m_repo);
            m_repo = 0;
            return false;
        }
        err = m_repo->open();
    } else {
        err = m_repo->open();
        if (!err.success) {
            QMessageBox::critical(this, tr("Repository error"),
                                 tr("Repository open failed:\n") + err.message);
            deleteRepository(m_repo);
            m_repo = 0;
            return false;
        }
        err = m_repo->sync();
    }

    if (!err.success)
    {
        QMessageBox::critical(this, tr("Repository error"),
                             tr("Repository synchronization failed:\n") + err.message);

        deleteRepository(m_repo);
        m_repo = 0;
        return false;
    }

    return true;
}


void MainWindow::loadScripts()
{
    basicScriptsModel->clear();
    customScriptsModel->clear();

    IniSettingReader *reader = new IniSettingReader();
    ScriptLoader loader(reader);

    QString configPath = QDir(QString(CONFIG_DIR)).absoluteFilePath("app_config.ini");
    QString headerPath = QDir(QString(CONFIG_DIR)).absoluteFilePath("header_ref.ini");

    if (!loader.loadConfig(configPath)) {
       QMessageBox::critical(this, tr("Config error"), tr("Configuration file could not be loaded:\n") + configPath);
            delete reader;
            return;
        }

    if (!loader.loadHeader(headerPath)) {
       QMessageBox::critical(this,tr("Header ref error"),tr("Header reference file could not be loaded:\n") + headerPath);
            delete reader;
            return;
        }

    QList<FindFileInfo> files = loader.scanSourcesAll();
    QList<FindFileInfo> validFiles;
    QStringList invalidHeader;

    for (int i = 0; i < files.size(); ++i) {
        const FindFileInfo &fileInfo = files.at(i);
        if (!fileInfo.headerCorrect) {
            invalidHeader.append(fileInfo.fileName);
            continue;
        }
        validFiles.append(fileInfo);
    }

    basicScriptsModel->setFiles(validFiles);
    customScriptsModel->setFiles(validFiles);
    resetFilterState();
    loadState();

    if(validFiles.size()!= files.size()){
        QMessageBox::warning(this,tr("Invalid script headers"), tr("These files contain incorrect headers and could not be displayed:\n") +  invalidHeader.join("\n"));
    }
    delete reader;
}



void MainWindow::openSelectedScript()
{    QModelIndex index = ui->listViewBasic->currentIndex();
    if (!index.isValid())
        return;

    QString scriptPath = index.data(ViewModel::FilePathRole).toString();
    if (scriptPath.isEmpty())
        return;

    QString baseWinPath = QApplication::applicationDirPath() + "/plugins/BaseWin.dll";

    ScriptWindow *w = new ScriptWindow;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->openScriptUI(scriptPath, baseWinPath);
    w->show();
}


void MainWindow::handleScriptDoubleClick(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    openSelectedScript();
}

void MainWindow::resetFilterState() {
    basicFilterModel->resetScriptFilters();
    customFilterModel->resetScriptFilters();

    deviceComboModel->clearFilters();
    roleComboModel->clearFilters();
    stageComboModel->clearFilters();
    categoryComboModel->clearFilters();

    ui->dComboBox->setCurrentIndex(0);
    ui->rComboBox->setCurrentIndex(0);
    ui->sComboBox->setCurrentIndex(0);
    ui->categoryComboBox->setCurrentIndex(0);
}

void MainWindow::applyTextSearch(const QString& text){

    ui->categoryComboBox->blockSignals(true);
    ui->categoryComboBox->setCurrentIndex(0);
    ui->categoryComboBox->blockSignals(false);

    customFilterModel->setCategoryFilter(QString());
    customFilterModel->setTextSearch(text);
}

void MainWindow::applyStageFilter()
{
    QString stage;
    if (ui->sComboBox->currentIndex() >= 0)
    {
        stage = ui->sComboBox->itemData(ui->sComboBox->currentIndex(), Qt::UserRole).toString();
    }
    if (stage == "__ALL__")
    {
        stage.clear();
    }
    basicFilterModel->setStageFilter(stage);
}

void MainWindow::applyDeviceFilter()
{
    QString device;
    if (ui->dComboBox->currentIndex() >= 0)
        device = ui->dComboBox->itemData(ui->dComboBox->currentIndex(), Qt::UserRole).toString();

    if (device == "__ALL__")
        device.clear();

    QString oldRole;

    if (ui->rComboBox->currentIndex() >= 0)
        oldRole = ui->rComboBox->itemData(ui->rComboBox->currentIndex(), Qt::UserRole).toString();

    if (oldRole == "__ALL__")
        oldRole.clear();

    QString oldStage;
    if (ui->sComboBox->currentIndex() >= 0)
        oldStage = ui->sComboBox->itemData(ui->sComboBox->currentIndex(), Qt::UserRole).toString();

    basicFilterModel->setDeviceFilter(device);
    roleComboModel->setFilter(ViewModel::DeviceColumn, device);

    int roleIndex = ui->rComboBox->findData(oldRole, Qt::UserRole);
    if (roleIndex >= 0)
        ui->rComboBox->setCurrentIndex(roleIndex);
    else {
        ui->rComboBox->setCurrentIndex(0);
        oldRole.clear();
    }

    stageComboModel->clearFilters();
    stageComboModel->setFilter(ViewModel::DeviceColumn, device);

    if (!oldRole.isEmpty())
        stageComboModel->setFilter(ViewModel::RoleColumn, oldRole);

    int stageIndex = ui->sComboBox->findData(oldStage, Qt::UserRole);
    if (stageIndex >= 0)
        ui->sComboBox->setCurrentIndex(stageIndex);
    else ui->sComboBox->setCurrentIndex(0);
}

void MainWindow::applyRoleFilter()
{
    QString role;
    if (ui->rComboBox->currentIndex() >= 0)
        role = ui->rComboBox->itemData(ui->rComboBox->currentIndex(), Qt::UserRole).toString();

    if (role == "__ALL__") role.clear();

    QString oldStage;
    if (ui->sComboBox->currentIndex() >= 0)
        oldStage = ui->sComboBox->itemData(ui->sComboBox->currentIndex(), Qt::UserRole).toString();

    basicFilterModel->setRoleFilter(role);

    QString device;
    if (ui->dComboBox->currentIndex() >= 0)
        device = ui->dComboBox->itemData(ui->dComboBox->currentIndex(), Qt::UserRole).toString();

    if (device == "__ALL__") device.clear();

    stageComboModel->clearFilters();

    if (!device.isEmpty())
        stageComboModel->setFilter(ViewModel::DeviceColumn, device);

    if (!role.isEmpty())
        stageComboModel->setFilter(ViewModel::RoleColumn, role);

    int stageIndex = ui->sComboBox->findData(oldStage, Qt::UserRole);
    if (stageIndex >= 0) ui->sComboBox->setCurrentIndex(stageIndex);
    else ui->sComboBox->setCurrentIndex(0);
}

void MainWindow::applyCategoryFilter()
{
   QString category;
   if (ui->categoryComboBox->currentIndex() >= 0)
       category = ui->categoryComboBox->itemData(ui->categoryComboBox->currentIndex(), Qt::UserRole).toString();

   if (category == "__ALL__") category.clear();
   customFilterModel->setCategoryFilter(category);
}

void MainWindow::showBasicPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);
}

void MainWindow::showCustomPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCustom);
}

void MainWindow::showCustomContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->listViewCustom->indexAt(pos);
    if (!index.isValid()) return;

    QMenu contextMenu(this);
    QAction* openDiffAction = contextMenu.addAction(tr("show changes"));

    QPoint globalPos = ui->listViewCustom->viewport()->mapToGlobal(pos);
    QAction* selectedAction = contextMenu.exec(globalPos);

    if (selectedAction == openDiffAction)
    {
        openDiffForIndex(index);
    }
}

void MainWindow::openDiffForIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    QString scriptPath = index.data(ViewModel::FilePathRole).toString();

    if (scriptPath.isEmpty()) {
        return;
    }

    DiffViewerWindow *diffWindow = new DiffViewerWindow(this);
    diffWindow->setAttribute(Qt::WA_DeleteOnClose);
    diffWindow->setWindowTitle(tr("changes"));
    diffWindow->setFilePath(scriptPath);
    diffWindow->show();
}

void MainWindow::showBasicContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->listViewBasic->indexAt(pos);
    if (!index.isValid()) return;

    QMenu contextMenu(this);
    QAction* openHistoryAction = contextMenu.addAction(tr("show history"));

    QPoint globalPos = ui->listViewBasic->viewport()->mapToGlobal(pos);
    QAction *selectedAction = contextMenu.exec(globalPos);

    if (selectedAction == openHistoryAction)
    {
        openHistoryForIndex(index);
    }
}

QVector<GuiCommitInfo> convertCommitInfoToGuiCommitInfo(const QList<CommitInfo>& backendList)
{
    QVector<GuiCommitInfo> guiHistory;
    guiHistory.reserve(backendList.size());

    for (int i = 0; i < backendList.size(); ++i)
    {
        const CommitInfo &c = backendList.at(i);

        GuiCommitInfo g;
        g.dateTime = c.commitCreateTime;
        g.author = c.authorName;
        g.authorEmail = c.authorEmail;
        g.commitMessage = c.commitMessage;
        g.commitHash = c.commitHash;

        guiHistory.append(g);
    }
    return guiHistory;
}

void MainWindow::openHistoryForIndex(const QModelIndex &index)
{
    QString scriptPath = index.data(ViewModel::FilePathRole).toString();
    if (scriptPath.isEmpty())
        return;

    if (!m_repo || m_repoRoot.isEmpty())
    {
        QMessageBox::warning(this, tr("Repository error"),
                             tr("Repository is not initialized."));
        return;
    }

    QString relPath = QDir(m_repoRoot).relativeFilePath(scriptPath);
    relPath.replace('\\', '/');

    if (relPath.startsWith(".."))
    {
        QMessageBox::warning(this, tr("Repository error"),
            tr("Selected file is outside the repository:\n%1").arg(scriptPath));
        return;
    }

    QList<CommitInfo> backendList;
    GitError err = m_repo->fillLog(backendList, relPath);
    if (!err.success)
    {
        QMessageBox::warning(this, tr("Git log error"), err.message);
        return;
    }

    QVector<GuiCommitInfo> guiHistory =
            convertCommitInfoToGuiCommitInfo(backendList);

    QString key = QFileInfo(scriptPath).absoluteFilePath();

    if (m_historyWindows.contains(key))
    {
        HistoryWindow* w = m_historyWindows.value(key);
        w->updateData(scriptPath, guiHistory);
        w->show();
        w->raise();
        w->activateWindow();
        return;
    }

    HistoryWindow* w = new HistoryWindow(scriptPath, guiHistory);
    w->setAttribute(Qt::WA_DeleteOnClose);

    w->setProperty("historyKey", key);
    connect(w, SIGNAL(destroyed(QObject*)),
            this, SLOT(onHistoryWindowDestroyed(QObject*)));

    m_historyWindows.insert(key, w);
    w->show();
}

void MainWindow::onHistoryWindowDestroyed(QObject* obj)
{
    QString key = obj->property("historyKey").toString();
    m_historyWindows.remove(key);
}
