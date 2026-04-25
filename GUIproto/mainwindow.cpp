#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scriptloader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "scriptwindow.h"

#include <QDir>
#include <QFileInfoList>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scriptsModel(new ViewModel(this)),
    filterModel(new ScriptFilterModel(this)),
    deviceComboModel(new ComboFilterModel(this)),
    roleComboModel(new ComboFilterModel(this)),
    stadeComboModel(new ComboFilterModel(this)),
    categoryComboModel(new ComboFilterModel(this))
{
    ui->setupUi(this);

    filterModel->setSourceModel(scriptsModel);


    ui->listViewBasic->setModel(filterModel);
    ui->listViewBasic->setModelColumn(ViewModel::DisplayColumn);

    ui->listViewCustom->setModel(filterModel);
    ui->listViewCustom->setModelColumn(ViewModel::DisplayColumn);

    deviceComboModel->setSourceModel(scriptsModel);
    deviceComboModel->setTargetColumn(ViewModel::DeviceColumn);
    ui->dComboBox->setModel(deviceComboModel);

    roleComboModel->setSourceModel(scriptsModel);
    roleComboModel->setTargetColumn(ViewModel::RoleColumn);
    ui->rComboBox->setModel(roleComboModel);

    stadeComboModel->setSourceModel(scriptsModel);
    stadeComboModel->setTargetColumn(ViewModel::StadeColumn);
    ui->sComboBox->setModel(stadeComboModel);

    categoryComboModel->setSourceModel(scriptsModel);
    categoryComboModel->setTargetColumn(ViewModel::CategoryColumn);
    ui->categoryComboBox->setModel(categoryComboModel);

    setWindowTitle("SE2");

    qDebug() << QCoreApplication::applicationDirPath();

    buildLayouts();

    loadScripts();

    connect(ui->listViewBasic, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(handleScriptDoubleClick(QModelIndex)));

    connect(ui->pushButton, SIGNAL(clicked()),
                this, SLOT(openSelectedScript()));

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(applyTextSearch(QString)));
    connect(ui->dComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(applyDeviceFilter()));
    connect(ui->rComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyRoleFilter()));
    connect(ui->sComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyStadeFilter()));
    connect(ui->categoryComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyCategoryFilter()));

    ui->radioBasic->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);

    connect(ui->radioBasic, SIGNAL(clicked()), this, SLOT(showBasicPage()));
    connect(ui->radioCustom, SIGNAL(clicked()), this, SLOT(showCustomPage()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


// временная загрузка для проверки обхода директории
void MainWindow::loadScripts()
{
    scriptsModel->clear();

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

    scriptsModel->setFiles(validFiles);
    resetFilterState();

    if(validFiles.size()!= files.size()){ //файлы с шапкой, не прошедшей валидацию не отображаются
        QMessageBox::warning(this,"Invalid script headers", "This files contain incorrect headers and could not be displayed:\n" +  invalidHeader.join("\n"));
    }

    delete reader;
}



void MainWindow::openSelectedScript()
{    QModelIndex index = ui->listViewBasic->currentIndex();
    if (!index.isValid())
        return;

    QString scriptPath = index.data(Qt::UserRole).toString();
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

void MainWindow::resetComboBoxes() {
    ui->dComboBox->blockSignals(true);
    ui->rComboBox->blockSignals(true);
    ui->sComboBox->blockSignals(true);
    ui->categoryComboBox->blockSignals(true);

    ui->dComboBox->setCurrentIndex(0);
    ui->rComboBox->setCurrentIndex(0);
    ui->sComboBox->setCurrentIndex(0);
    ui->categoryComboBox->setCurrentIndex(0);

    ui->dComboBox->blockSignals(false);
    ui->rComboBox->blockSignals(false);
    ui->sComboBox->blockSignals(false);
    ui->categoryComboBox->blockSignals(false);
}

void MainWindow::resetFilterState() {
    filterModel->resetScriptFilters();

    deviceComboModel->clearFilters();
    roleComboModel->clearFilters();
    stadeComboModel->clearFilters();
    categoryComboModel->clearFilters();

    resetComboBoxes();
}

void MainWindow::applyTextSearch(const QString& text)
{
    ui->categoryComboBox->blockSignals(true);
    ui->categoryComboBox->setCurrentIndex(0);
    ui->categoryComboBox->blockSignals(false);

    filterModel->setCategoryFilter(QString());
    filterModel->setTextSearch(text);
}

void MainWindow::applyStadeFilter()
{
    QString stade = ui->sComboBox->currentText();
    filterModel->setStadeFilter(stade);

}

void MainWindow::applyDeviceFilter() {
    QString device = ui->dComboBox->currentText();
    filterModel->setDeviceFilter(device);
    roleComboModel->setFilter(ViewModel::DeviceColumn, device);
    stadeComboModel->setFilter(ViewModel::DeviceColumn, device);

    ui->rComboBox->setCurrentIndex(0);
    ui->sComboBox->setCurrentIndex(0);
}

void MainWindow::applyRoleFilter() {
    QString role;
    if (ui->rComboBox->currentIndex() >= 0) {
       role = ui->rComboBox->itemData(ui->rComboBox->currentIndex(),Qt::UserRole).toString();
    }
    filterModel->setRoleFilter(role);
    stadeComboModel->clearFilters();
    QString device = ui->dComboBox->currentText();

    stadeComboModel->setFilter(ViewModel::DeviceColumn, device);
    stadeComboModel->setFilter(ViewModel::RoleColumn, role);

    ui->sComboBox->setCurrentIndex(0);
}

void MainWindow::applyCategoryFilter() {
    QString category = ui->categoryComboBox->currentText();
    filterModel->setCategoryFilter(category);
}

void MainWindow::buildLayouts()
{
    const int outerMargin = 20;
    const int sideSpacing = 8;
    const int sideWidth = 54;
    const int reserveRight = outerMargin + sideSpacing + sideWidth;

    QVBoxLayout *centralLayout = new QVBoxLayout(ui->centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(ui->modeWidget);


    QVBoxLayout *modeLayout = new QVBoxLayout(ui->modeWidget);
    modeLayout->setContentsMargins(0, 0, 0, 0);
    modeLayout->setSpacing(4);

    QHBoxLayout *modeBarLayout = new QHBoxLayout(ui->modeBarWidget);
    modeBarLayout->setContentsMargins(0, 0, 0, 0);
    modeBarLayout->setSpacing(12);
    modeBarLayout->addStretch();
    modeBarLayout->addWidget(ui->radioBasic);
    modeBarLayout->addWidget(ui->radioCustom);
    modeBarLayout->addStretch();

    modeLayout->addWidget(ui->modeBarWidget, 0);
    modeLayout->addWidget(ui->stackedWidget, 1);

    QVBoxLayout *basicLayout = new QVBoxLayout(ui->pageBasic);
    basicLayout->setContentsMargins(0, 0, 0, 0);
    basicLayout->setSpacing(6);

    basicLayout->addWidget(ui->topFrameRandD, 0);
    basicLayout->addWidget(ui->searchWidget, 0);
    basicLayout->addWidget(ui->contentWidgetBasic, 1);
    basicLayout->addWidget(ui->bottomWidgetBasic, 0);

    // topFrameRandD
    QHBoxLayout *topLayout = new QHBoxLayout(ui->topFrameRandD);
    topLayout->setContentsMargins(outerMargin, 0, reserveRight, 0);
    topLayout->setSpacing(8);

    topLayout->addWidget(ui->dLabel);
    topLayout->addWidget(ui->dComboBox, 1);

    // searchWidget
    QHBoxLayout *searchLayout = new QHBoxLayout(ui->searchWidget);
    searchLayout->setContentsMargins(outerMargin, 0, reserveRight, 0);
    searchLayout->setSpacing(10);

    searchLayout->addWidget(ui->rLabel);
    searchLayout->addWidget(ui->rComboBox, 1);

    searchLayout->addSpacing(30);

    searchLayout->addWidget(ui->sLabel);
    searchLayout->addWidget(ui->sComboBox, 1);

    // contentWidgetBasic
    QHBoxLayout *contentLayout = new QHBoxLayout(ui->contentWidgetBasic);
    contentLayout->setContentsMargins(outerMargin, 0, outerMargin, 0);
    contentLayout->setSpacing(sideSpacing);

    contentLayout->addWidget(ui->listViewBasic, 1);
    contentLayout->addWidget(ui->sideWidgetBasic, 0);

    // sideWidgetBasic
    QVBoxLayout *sideLayout = new QVBoxLayout(ui->sideWidgetBasic);
    sideLayout->setContentsMargins(4, 0, 0, 0);
    sideLayout->setSpacing(8);
    sideLayout->addWidget(ui->editButton, 0, Qt::AlignTop | Qt::AlignHCenter);
    sideLayout->addStretch();

    // bottomWidgetBasic
    QHBoxLayout *bottomLayout = new QHBoxLayout(ui->bottomWidgetBasic);
    bottomLayout->setContentsMargins(outerMargin, 8, reserveRight, 8);
    bottomLayout->setSpacing(0);
    bottomLayout->addStretch();
    bottomLayout->addWidget(ui->pushButton, 0, Qt::AlignCenter);
    bottomLayout->addStretch();

    QVBoxLayout *customLayout = new QVBoxLayout(ui->pageCustom);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->setSpacing(6);

    customLayout->addWidget(ui->topFrameCategory, 0);
    customLayout->addWidget(ui->searchWidget_2, 0);
    customLayout->addWidget(ui->contentWidgetCustom, 1);
    customLayout->addWidget(ui->bottomWidgetCustom, 0);

    // topFrameCategory
    QHBoxLayout *topLayoutCat = new QHBoxLayout(ui->topFrameCategory);
    topLayoutCat->setContentsMargins(20, 0, 100, 0);
    topLayoutCat->setSpacing(5);

    topLayoutCat->addWidget(ui->cLabel);
    topLayoutCat->addWidget(ui->categoryComboBox, 1);

    // searchWidget_2
    QHBoxLayout *searchLayout_2 = new QHBoxLayout(ui->searchWidget_2);
    searchLayout_2->setContentsMargins(outerMargin, 0, reserveRight, 0);
    searchLayout_2->setSpacing(0);
    searchLayout_2->addWidget(ui->lineEdit, 1);

    // contentWidgetCustom
    QHBoxLayout *contentLayoutCustom = new QHBoxLayout(ui->contentWidgetCustom);
    contentLayoutCustom->setContentsMargins(outerMargin, 0, outerMargin, 0);
    contentLayoutCustom->setSpacing(sideSpacing);

    contentLayoutCustom->addWidget(ui->listViewCustom, 1);
    contentLayoutCustom->addWidget(ui->sideWidgetCustom, 0);

    // sideWidgetCustom
    QVBoxLayout *sideLayoutCustom = new QVBoxLayout(ui->sideWidgetCustom);
    sideLayoutCustom->setContentsMargins(4, 0, 0, 0);
    sideLayoutCustom->setSpacing(8);
    sideLayoutCustom->addWidget(ui->editButton_2, 0, Qt::AlignTop | Qt::AlignHCenter);
    sideLayoutCustom->addWidget(ui->presetButton, 0, Qt::AlignTop | Qt::AlignHCenter);
    sideLayoutCustom->addStretch();

    // bottomWidgetCustom
    QHBoxLayout *bottomLayoutCustom = new QHBoxLayout(ui->bottomWidgetCustom);
    bottomLayoutCustom->setContentsMargins(outerMargin, 8, reserveRight, 8);
    bottomLayoutCustom->setSpacing(0);
    bottomLayoutCustom->addStretch();
    bottomLayoutCustom->addWidget(ui->pushButton_2, 0, Qt::AlignCenter);
    bottomLayoutCustom->addStretch();

    ui->modeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->pageBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->pageCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->topFrameRandD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->topFrameCategory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->searchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->searchWidget_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->contentWidgetBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->contentWidgetCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->bottomWidgetBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->bottomWidgetCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->listViewBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->listViewCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->sideWidgetBasic->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->sideWidgetCustom->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->editButton_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->presetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->dComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->rComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->sComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->modeBarWidget->setMinimumHeight(51);
    ui->topFrameRandD->setMinimumHeight(50);
    ui->topFrameCategory->setMinimumHeight(50);
    ui->searchWidget->setMinimumHeight(41);
    ui->searchWidget_2->setMinimumHeight(24);
    ui->bottomWidgetBasic->setMinimumHeight(61);
    ui->bottomWidgetCustom->setMinimumHeight(61);

    ui->categoryComboBox->setMinimumWidth(200);

    ui->dComboBox->setMinimumWidth(220);
    ui->rComboBox->setMinimumWidth(110);
    ui->sComboBox->setMinimumWidth(90);

    ui->sideWidgetBasic->setFixedWidth(sideWidth);
    ui->sideWidgetCustom->setFixedWidth(sideWidth);

    ui->editButton->setFixedSize(50, 50);
    ui->editButton_2->setFixedSize(50, 50);
    ui->presetButton->setFixedSize(50, 50);

    ui->pushButton->setFixedSize(121, 31);
    ui->pushButton_2->setFixedSize(121, 31);

}

//переключение режимов
void MainWindow::showBasicPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);

    scriptsModel->setViewMode(ViewModel::basicMode);
    filterModel->setMode(ViewModel::basicMode);
    resetFilterState();

}

void MainWindow::showCustomPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCustom);

    scriptsModel->setViewMode(ViewModel::customMode);
    filterModel->setMode(ViewModel::customMode);
    resetFilterState();

}
