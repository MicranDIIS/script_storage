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
    filterModel(new ScriptFilterModel(this))
{
    ui->setupUi(this);

    filterModel->setSourceModel(scriptsModel);

    ui->listViewBasic->setModel(filterModel);
    ui->listViewCustom->setModel(filterModel);
    setWindowTitle("SE2");

    qDebug() << QCoreApplication::applicationDirPath();

    buildLayouts();

    loadScripts();

    connect(ui->listViewBasic, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(handleScriptDoubleClick(QModelIndex)));

    connect(ui->pushButton, SIGNAL(clicked()),
                this, SLOT(openSelectedScript()));

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(applyTextSearch(QString)));
    connect(ui->rComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyRoleFilter()));
    connect(ui->sComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(applyStadeFilter()));
    connect(ui->dComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(applyDeviceFilter()));
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
    if(validFiles.size()!= files.size()){ //файлы с шапкой, не прошедшей валидацию не отображаются
        QMessageBox::warning(this,"Invalid script headers", "This files contain incorrect headers and could not be displayed:\n" +  invalidHeader.join("\n"));
    }

    fillComboBox(ui->categoryComboBox, files, "categories", false); // заполняем значения фильтров
    fillComboBox(ui->rComboBox, files, "roles", true);
    fillComboBox(ui->dComboBox, files, "devices", false);
    fillComboBox(ui->sComboBox, files, "stades", true);

    delete reader;
}


//метод для наполнения комбобоксов
//из полей шапки значения записываем в фильтр
void MainWindow::fillComboBox(QComboBox *comboBox,const QList<FindFileInfo>& files,const QString& fieldName,bool translateValues)
{
    if (!comboBox) {
        return;
    }

    QStringList values;

    comboBox->clear();

    for (int i = 0; i < files.size(); ++i) {
        const FindFileInfo& info = files.at(i);

        if (fieldName == "categories") {
            QString value = info.categories;

            if (!value.isEmpty() && !values.contains(value)) {
                values.append(value);
            }
        }
        else if (fieldName == "roles") {
            for (int j = 0; j < info.roles.size(); ++j) {
                QString value = info.roles.at(j);

                if (!value.isEmpty() && !values.contains(value)) {
                    values.append(value);
                }
            }
        }
        else if (fieldName == "devices") {
            for (int j = 0; j < info.devices.size(); ++j) {
                QString value = info.devices.at(j);

                if (!value.isEmpty() && !values.contains(value)) {
                    values.append(value);
                }
            }
        }
        else if (fieldName == "stades") {
            for (int j = 0; j < info.stades.size(); ++j) {
                QString value = info.stades.at(j);

                if (!value.isEmpty() && !values.contains(value)) {
                    values.append(value);
                }
            }
        }
    }

    values.sort();

    for (int i = 0; i < values.size(); ++i) {
        QString value = values.at(i);
        QString visibleText = value;

        if (translateValues) { // перевод метаданных для гуи - костыльно, но групп пока не так много вроде :)
            if (value == "developer")
                visibleText = QString::fromUtf8("Разработчик");
            else if (value == "production")
                visibleText = QString::fromUtf8("Производство");
            else if (value == "metrolog")
                visibleText = QString::fromUtf8("Метролог");
            else if (value == "PSI")
                visibleText = QString::fromUtf8("ПСИ");
            else if (value == "other")
                visibleText = QString::fromUtf8("Другое");
            else if (value == "test")
                visibleText = QString::fromUtf8("Тест");
            else if (value == "calibration")
                visibleText = QString::fromUtf8("Калибровка");
        }
        comboBox->addItem(visibleText, value);
    }
    comboBox->setCurrentIndex(-1);
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

void MainWindow::applyTextSearch(const QString& text)
{
    ui->rComboBox->blockSignals(true);
    ui->dComboBox->blockSignals(true);
    ui->sComboBox->blockSignals(true);
    ui->categoryComboBox->blockSignals(true);

    ui->rComboBox->setCurrentIndex(-1);
    ui->dComboBox->setCurrentIndex(-1);
    ui->sComboBox->setCurrentIndex(-1);
    ui->categoryComboBox->setCurrentIndex(-1);

    ui->rComboBox->blockSignals(false);
    ui->dComboBox->blockSignals(false);
    ui->sComboBox->blockSignals(false);
    ui->categoryComboBox->blockSignals(false);

    filterModel->resetAllFilters();
    filterModel->setTextSearch(text);
}

void MainWindow::applyStadeFilter()
{
    QString stade = ui->sComboBox->itemData(ui->sComboBox->currentIndex()).toString();
    filterModel->setStadeFilter(stade);
}

void MainWindow::applyDeviceFilter()
{
    QString device = ui->dComboBox->itemData(ui->dComboBox->currentIndex()).toString();
    filterModel->setDeviceFilter(device);
}

void MainWindow::applyRoleFilter()
{
    QString role = ui->rComboBox->itemData(ui->rComboBox->currentIndex()).toString();
    filterModel->setRoleFilter(role);
}

void MainWindow::applyCategoryFilter()
{
    QString category = ui->categoryComboBox->itemData(ui->categoryComboBox->currentIndex()).toString();
    filterModel->setCategoryFilter(category);
}

void MainWindow::buildLayouts()
{
    //centralWidget
    QVBoxLayout *centralLayout = new QVBoxLayout(ui->centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    centralLayout->addWidget(ui->modeWidget);

    //modeWidget
    QVBoxLayout *modeLayout = new QVBoxLayout(ui->modeWidget);
    modeLayout->setContentsMargins(0, 0, 0, 0);
    modeLayout->setSpacing(4);

    //modeBarWidget
    QHBoxLayout *modeBarLayout = new QHBoxLayout(ui->modeBarWidget);
    modeBarLayout->setContentsMargins(0, 0, 0, 0);
    modeBarLayout->setSpacing(12); modeBarLayout->addStretch();
    modeBarLayout->addWidget(ui->radioBasic);
    modeBarLayout->addWidget(ui->radioCustom);
    modeBarLayout->addStretch();

    modeLayout->addWidget(ui->modeBarWidget, 0);
    modeLayout->addWidget(ui->stackedWidget, 1);

    //pageBasic
    QVBoxLayout *basicLayout = new QVBoxLayout(ui->pageBasic);
    basicLayout->setContentsMargins(0, 0, 0, 0);
    basicLayout->setSpacing(6);

    basicLayout->addWidget(ui->topFrameRandD, 0);
    basicLayout->addWidget(ui->searchWidget, 0);
    basicLayout->addWidget(ui->contentWidgetBasic, 1);
    basicLayout->addWidget(ui->bottomWidgetBasic, 0);

    //topFrameRandD
    QHBoxLayout *topLayout = new QHBoxLayout(ui->topFrameRandD);
    topLayout->setContentsMargins(8, 0, 8, 0);
    topLayout->setSpacing(8);

    topLayout->addWidget(ui->rLabel);
    topLayout->addWidget(ui->rComboBox);
    topLayout->addSpacing(12);
    topLayout->addWidget(ui->dLabel);
    topLayout->addWidget(ui->dComboBox);

    //searchWidget
    QHBoxLayout *searchLayout = new QHBoxLayout(ui->searchWidget);
    searchLayout->setContentsMargins(20, 8, 20, 8);
    searchLayout->setSpacing(8);

    searchLayout->addWidget(ui->sLabel);
    searchLayout->addWidget(ui->sComboBox);
    searchLayout->addSpacing(12);
    searchLayout->addWidget(ui->lineEdit);
    searchLayout->addStretch();

    //contentWidgetBasic
    QHBoxLayout *contentLayout = new QHBoxLayout(ui->contentWidgetBasic);
    contentLayout->setContentsMargins(20, 0, 20, 0);
    contentLayout->setSpacing(8);

    contentLayout->addWidget(ui->listViewBasic, 1);
    contentLayout->addWidget(ui->sideWidgetBasic, 0);

     //sideWidgetBasic
    QVBoxLayout *sideLayout = new QVBoxLayout(ui->sideWidgetBasic);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    sideLayout->addWidget(ui->editButton, 0, Qt::AlignTop | Qt::AlignHCenter);
    sideLayout->addStretch();

     //bottomWidgetBasic
    QHBoxLayout *bottomLayout = new QHBoxLayout(ui->bottomWidgetBasic);
    bottomLayout->setContentsMargins(0, 8, 0, 8);
    bottomLayout->setSpacing(0);

    bottomLayout->addStretch();
    bottomLayout->addWidget(ui->pushButton, 0, Qt::AlignCenter);
    bottomLayout->addStretch();

    //pageCustom
    QVBoxLayout *customLayout = new QVBoxLayout (ui->pageCustom);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->setSpacing(6);

    customLayout->addWidget(ui->topFrameCategory, 0);
    customLayout->addWidget(ui->contentWidgetCustom, 0);
    customLayout->addWidget(ui->bottomWidgetCustom, 0);

    //topFrameCategory
    QHBoxLayout *topLayoutCat = new QHBoxLayout(ui->topFrameCategory);
    topLayoutCat->setContentsMargins(20, 0, 100, 0);
    topLayoutCat->setSpacing(5);

    topLayoutCat->addWidget(ui->cLabel);
    topLayoutCat->addWidget(ui->categoryComboBox, 1);


    //contentWidgetCustom
    QHBoxLayout *contentLayoutCustom = new QHBoxLayout(ui->contentWidgetCustom);
    contentLayoutCustom->setContentsMargins(20, 0, 20, 0);
    contentLayoutCustom->setSpacing(8);

    contentLayoutCustom->addWidget(ui->listViewCustom, 1);
    contentLayoutCustom->addWidget(ui->sideWidgetCustom, 0);

     //sideWidgetCustom
    QVBoxLayout *sideLayoutCustom = new QVBoxLayout(ui->sideWidgetCustom);
    sideLayoutCustom->setContentsMargins(0, 0, 0, 0);
    sideLayoutCustom->setSpacing(0);

    sideLayoutCustom->addWidget(ui->editButton_2, 0, Qt::AlignTop | Qt::AlignHCenter);
    sideLayoutCustom->addStretch();


    //bottomWidgetBasic
    QHBoxLayout *bottomLayoutCustom = new QHBoxLayout(ui->bottomWidgetCustom);
    bottomLayoutCustom->setContentsMargins(0, 8, 0, 8);
    bottomLayoutCustom->setSpacing(0);

    bottomLayoutCustom->addStretch();
    bottomLayoutCustom->addWidget(ui->pushButton_2, 0, Qt::AlignCenter);
    bottomLayoutCustom->addStretch();



    //sizePolicy
    ui->modeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->pageBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->pageCustom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ui->topFrameRandD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->topFrameCategory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->searchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->contentWidgetBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->bottomWidgetBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->contentWidgetCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->bottomWidgetCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->listViewBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->listViewCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->sideWidgetBasic->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->sideWidgetCustom->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->editButton_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->modeBarWidget->setMinimumHeight(51);
    ui->topFrameRandD->setMinimumHeight(50);
    ui->topFrameCategory->setMinimumHeight(50);
    ui->searchWidget->setMinimumHeight(41);
    ui->bottomWidgetBasic->setMinimumHeight(61);
    ui->bottomWidgetCustom->setMinimumHeight(61);
    ui->categoryComboBox->setMinimumWidth(200);

    ui->sideWidgetBasic->setFixedWidth(70);
    ui->sideWidgetCustom->setFixedWidth(70);
    ui->editButton->setFixedSize(50, 50);
    ui->pushButton->setFixedSize(121, 31);
    ui->editButton_2->setFixedSize(50, 50);
    ui->pushButton_2->setFixedSize(121, 31);

}

//переключение режимов
void MainWindow::showBasicPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);
    scriptsModel->setViewMode(ViewModel::basicMode);
    filterModel->setMode(ViewModel::basicMode);

}

void MainWindow::showCustomPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCustom);
    scriptsModel->setViewMode(ViewModel::customMode);
    filterModel->setMode(ViewModel::customMode);
}

