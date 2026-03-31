#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "scriptwindow.h"
#include <QDir>
#include <QFileInfoList>
#include <QTreeWidgetItem>

#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("SE2");

    buildLayouts();

//    loadScripts();

//    connect(ui->treeWidgetBasic, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
//            this, SLOT(handleScriptDoubleClick(QTreeWidgetItem*,int)));

    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(openSelectedScript()));

    ui->radioBasic->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);

    connect(ui->radioBasic, SIGNAL(clicked()), this, SLOT(showBasicPage()));
    connect(ui->radioCustom, SIGNAL(clicked()), this, SLOT(showCustomPage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::loadScripts()
//{
//    ui->treeWidgetBasic->clear();

//    QString scriptsPath = QCoreApplication::applicationDirPath() + "/scripts";
//    QDir dir(scriptsPath);

//    QFileInfoList files = dir.entryInfoList(QStringList() << "*.lua", QDir::Files);

//    foreach (const QFileInfo &fileInfo, files) {
//        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetBasic);
//        item->setText(0, fileInfo.baseName());
//        item->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
//    }
//}

//void MainWindow::openSelectedScript()
//{
//    QTreeWidgetItem *item = ui->treeWidgetBasic->currentItem();
//    if (!item)
//        return;

//    QString scriptPath = item->data(0, Qt::UserRole).toString();
//    if (scriptPath.isEmpty())
//        return;

//    QString baseWinPath = QApplication::applicationDirPath() + "/plugins/BaseWin.dll";

//    ScriptWindow *w = new ScriptWindow;
//    w->setAttribute(Qt::WA_DeleteOnClose);
//    w->openScriptUI(scriptPath, baseWinPath);
//    w->show();
//}

//void MainWindow::handleScriptDoubleClick(QTreeWidgetItem *item, int column)
//{
//    Q_UNUSED(item);
//    Q_UNUSED(column);
//    openSelectedScript();
//}

//компоновка
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
    searchLayout->setSpacing(0);

    searchLayout->addWidget(ui->lineEdit);
    searchLayout->addStretch();

    //contentWidgetBasic
    QHBoxLayout *contentLayout = new QHBoxLayout(ui->contentWidgetBasic);
    contentLayout->setContentsMargins(20, 0, 20, 0);
    contentLayout->setSpacing(8);

    contentLayout->addWidget(ui->treeViewBasic, 1);
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

    contentLayoutCustom->addWidget(ui->treeViewCustom, 1);
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

    ui->treeViewBasic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->treeViewCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

//переключение режима
void MainWindow::showBasicPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageBasic);
}

void MainWindow::showCustomPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCustom);
}

