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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    basicScriptsModel(new ViewModel(this)),
    customScriptsModel(new ViewModel(this)),

    basicFilterModel(new ScriptFilterModel(this)),
    customFilterModel(new ScriptFilterModel(this)),

    deviceComboModel(new ComboFilterModel(this)),
    roleComboModel(new ComboFilterModel(this)),
    stadeComboModel(new ComboFilterModel(this)),
    categoryComboModel(new ComboFilterModel(this)),

    m_repo(0)
{
    ui->setupUi(this);

    mgitlib.setFileName("mgit.dll");
    if (!mgitlib.load()){
        qDebug()<< mgitlib.errorString();
    }
    m_deleteRepository = (DeleteRepositoryFunc)mgitlib.resolve("deleteRepository");
    m_createRepository = (CreateRepositoryFunc)mgitlib.resolve("createRepository");

    if (!syncRepo())
            return;

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

    stadeComboModel->setSourceModel(basicScriptsModel);
    stadeComboModel->setTargetColumn(ViewModel::StadeColumn);
    ui->sComboBox->setModel(stadeComboModel);

    categoryComboModel->setSourceModel(customFilterModel);
    categoryComboModel->setTargetColumn(ViewModel::CategoryColumn);
    ui->categoryComboBox->setModel(categoryComboModel);

    setWindowTitle("SE2");

//    qDebug() << QCoreApplication::applicationDirPath();

    loadScripts();

    connect(ui->listViewBasic, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(handleScriptDoubleClick(QModelIndex)));
    connect(ui->pushButton, SIGNAL(clicked()),this, SLOT(openSelectedScript()));

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(applyTextSearch(QString)));
    connect(ui->dComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(applyDeviceFilter()));
    connect(ui->rComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyRoleFilter()));
    connect(ui->sComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyStadeFilter()));
    connect(ui->categoryComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyCategoryFilter()));

    ui->radioBasic->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);

    connect(ui->radioBasic, SIGNAL(clicked()), this, SLOT(showBasicPage()));
    connect(ui->radioCustom, SIGNAL(clicked()), this, SLOT(showCustomPage()));

    ui->listViewCustom->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listViewCustom, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(showCustomContextMenu(const QPoint &)));

    ui->listViewBasic->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listViewBasic, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(showBasicContextMenu(const QPoint &)));
}

MainWindow::~MainWindow()
{
    if (m_repo)
    {
        m_deleteRepository(m_repo);
        m_repo = 0;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){

    settings.beginGroup("mainwindow");

    settings.setValue("filters/device", ui->dComboBox->currentText());
    settings.setValue("filters/role",ui->rComboBox->itemData(ui->rComboBox->currentIndex(),Qt::UserRole).toString());
    settings.setValue("filters/stade",ui->sComboBox->currentText());
    settings.setValue("filters/category",ui->categoryComboBox->currentText());
    settings.setValue("win/geometry", saveGeometry());
    settings.setValue("win/state", saveState());
    settings.endGroup();

    QMainWindow::closeEvent(event);
}

void MainWindow::loadState(){  //добавить обработку пропавшего фильтра

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

    QString resStade = settings.value("filters/stade").toString();
    int stadeIndex = ui->sComboBox->findText(resStade);
    if (stadeIndex >= 0) {
        ui->sComboBox->setCurrentIndex(stadeIndex);
    }
    applyStadeFilter();

    QString resCategory = settings.value("filters/category").toString();
    int categoryIndex = ui->categoryComboBox->findText(resCategory);
    if (categoryIndex >= 0) {
        ui->categoryComboBox->setCurrentIndex(categoryIndex);
    }
    applyCategoryFilter();

    restoreGeometry(settings.value("win/geometry").toByteArray());
    settings.endGroup();
}

// синхронизация или клонирование репозитория в локальную папку(все пути в конфиге repo.ini указываем)
 bool MainWindow::syncRepo(){

     if (m_repo)
     {
         m_deleteRepository(m_repo);
         m_repo = 0;
     }

     QString appDir = QCoreApplication::applicationDirPath();
     QString repoConfigPath = QDir(appDir).absoluteFilePath("../../GUIproto/config/repo.ini");

//     qDebug() << "repo.ini path:" << repoConfigPath;

     IniSettingReader reader;
     RepoConfig repoConfig;

     if (!reader.repoLoad(repoConfigPath, repoConfig)) {
         QMessageBox::warning(this,"Config error", "Configuration file for remote could not be loaded:\n" + repoConfigPath);
         return false;
     }

     m_repoRoot = repoConfig.path;

//     qDebug() << "url:" << repoConfig.url;
//     qDebug() << "branch:" << repoConfig.branch;
//     qDebug() << "path:" << repoConfig.path;
//     qDebug() << "username:" << repoConfig.username;
//     qDebug() << "token is empty:" << repoConfig.token.isEmpty();

     QDir repoDir(repoConfig.path);
     QDir gitDir(repoDir.absoluteFilePath(".git"));

     if (repoDir.exists() && !gitDir.exists()) {
         QStringList entries = repoDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
         if (!entries.isEmpty()) {
             QMessageBox::warning(this,"Repository error", "Folder for remote repositiry is not empty.");
             return false;
         }
     }

     m_repo = m_createRepository(repoConfig);
     if (!m_repo)
     {
         QMessageBox::warning(this, "Repository error",
                              "Repository object could not be created.");
         return false;
     }

     Gerror err;

     if (!gitDir.exists()) {
         err = m_repo->clone();
         if (err.hasError()) {
             return false;
         }
         err = m_repo->open();
     } else {
         err = m_repo->open();
         err = m_repo->sync();
         }
     if (err.hasError())
     {
         QMessageBox::warning(this, "Repository error","Repository synchronization failed:\n" + err.getMsg());

         m_deleteRepository(m_repo);
         m_repo = 0;
         return false;
     }

//     qDebug() << "[syncRepo] ok, repoRoot =" << m_repoRoot << "repo ptr =" << m_repo;
     return true;
 }


// загрузка для проверки обхода директории
void MainWindow::loadScripts()
{
    basicScriptsModel->clear();
    customScriptsModel->clear();

    IniSettingReader *reader = new IniSettingReader();
    ScriptLoader loader(reader);

    QString appDir = QCoreApplication::applicationDirPath();

    QString configPath = QDir(appDir).absoluteFilePath("../../GUIproto/config/app_config.ini");
    QString headerPath = QDir(appDir).absoluteFilePath("../../GUIproto/config/header_ref.ini");

    if (!loader.configLoad(configPath)) {
       QMessageBox::warning(this,"Config error", "Configuration file could not be loaded:\n" + configPath);
            delete reader;
            return;
        }

    if (!loader.headerLoad(headerPath)) {
       QMessageBox::warning(this,"Header ref error","Header reference file could not be loaded:\n" + headerPath);
            delete reader;
            return;
        }

    QList<FindFileInfo> files = loader.scanSourcesAll();
//    qDebug() << "all scanned files =" << files.size();
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
//    qDebug() << "valid files =" << validFiles.size();

    basicScriptsModel->setFiles(validFiles);
    customScriptsModel->setFiles(validFiles);
    resetFilterState();
    loadState();

    if(validFiles.size()!= files.size()){ //файлы с шапкой, не прошедшей валидацию не отображаются
        QMessageBox::warning(this,"Invalid script headers", "This files contain incorrect headers and could not be displayed:\n" +  invalidHeader.join("\n"));
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
    stadeComboModel->clearFilters();
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

void MainWindow::applyStadeFilter(){

    QString stade = ui->sComboBox->currentText();
    basicFilterModel->setStadeFilter(stade);
}

void MainWindow::applyDeviceFilter() {

    QString device = ui->dComboBox->currentText();
    QString oldRole;
    if (ui->rComboBox->currentIndex() >= 0) {
        oldRole = ui->rComboBox->itemData(ui->rComboBox->currentIndex(), Qt::UserRole).toString();
    }

    QString oldStade = ui->sComboBox->currentText();
    basicFilterModel->setDeviceFilter(device);
    roleComboModel->setFilter(ViewModel::DeviceColumn, device);

    int roleIndex = ui->rComboBox->findData(oldRole, Qt::UserRole);
    if (roleIndex >= 0) {
        ui->rComboBox->setCurrentIndex(roleIndex);
    } else {
        ui->rComboBox->setCurrentIndex(0);
        oldRole.clear();
    }

    stadeComboModel->clearFilters();
    stadeComboModel->setFilter(ViewModel::DeviceColumn, device);

    if (!oldRole.isEmpty() && oldRole != QString::fromUtf8("Все")) {
        stadeComboModel->setFilter(ViewModel::RoleColumn, oldRole);
    }

    int stadeIndex = ui->sComboBox->findText(oldStade);
    if (stadeIndex >= 0) {
        ui->sComboBox->setCurrentIndex(stadeIndex);
    } else {
        ui->sComboBox->setCurrentIndex(0);
    }
}

void MainWindow::applyRoleFilter(){

    QString role;

    if (ui->rComboBox->currentIndex() >= 0) {
        role = ui->rComboBox->itemData(ui->rComboBox->currentIndex(), Qt::UserRole).toString();
    }
    QString oldStade = ui->sComboBox->currentText();
    basicFilterModel->setRoleFilter(role);

    QString device = ui->dComboBox->currentText();
    stadeComboModel->clearFilters();

    if (!device.isEmpty()) {
        stadeComboModel->setFilter(ViewModel::DeviceColumn, device);
    }

    if (!role.isEmpty()) {
        stadeComboModel->setFilter(ViewModel::RoleColumn, role);
    }

    int stadeIndex = ui->sComboBox->findText(oldStade);
    if (stadeIndex >= 0) {
        ui->sComboBox->setCurrentIndex(stadeIndex);
    } else {
        ui->sComboBox->setCurrentIndex(0);
    }

}

void MainWindow::applyCategoryFilter() {
    QString category = ui->categoryComboBox->currentText();
    customFilterModel->setCategoryFilter(category);
}

//переключение режимов
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
//    qDebug() << "showContextMenu called, pos =" << pos;

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
        g.dateTime = c.getAuthorDateTime();
        g.author = c.getAuthorName();
        g.authorEmail = c.getAuthorEmail();
        g.commitMessage = c.getCommitMsg();
        g.commitHash = c.getCommitHash();

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
    Gerror err = m_repo->log(backendList, relPath);
    if (err.hasError())
    {
        QMessageBox::warning(this, tr("Git log error"), err.getMsg());
        return;
    }

    QVector<GuiCommitInfo> guiHistory =
            convertCommitInfoToGuiCommitInfo(backendList);

    QString key = QFileInfo(scriptPath).absoluteFilePath();

    if (m_historyWindows.contains(key))
    {
        HistoryWindow* w = m_historyWindows.value(key);
        w->setHistory(guiHistory);
        w->show();
        w->raise();
        w->activateWindow();
        return;
    }

    HistoryWindow* w = new HistoryWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setFilePath(scriptPath);
    w->setHistory(guiHistory);
    m_historyWindows.insert(key, w);
    w->show();

    w->setProperty("historyKey", key);
    connect(w, SIGNAL(destroyed(QObject*)),
            this, SLOT(onHistoryWindowDestroyed(QObject*)));
}

void MainWindow::onHistoryWindowDestroyed(QObject* obj)
{
    QString key = obj->property("historyKey").toString();
    m_historyWindows.remove(key);
}

