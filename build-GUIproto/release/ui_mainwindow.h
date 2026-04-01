/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed 1. Apr 16:05:42 2026
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *modeWidget;
    QStackedWidget *stackedWidget;
    QWidget *pageBasic;
    QWidget *bottomWidgetBasic;
    QPushButton *pushButton;
    QWidget *contentWidgetBasic;
    QTreeView *treeViewBasic;
    QWidget *sideWidgetBasic;
    QToolButton *editButton;
    QWidget *searchWidget;
    QLineEdit *lineEdit;
    QFrame *topFrameRandD;
    QLabel *rLabel;
    QComboBox *rComboBox;
    QLabel *dLabel;
    QComboBox *dComboBox;
    QWidget *pageCustom;
    QFrame *topFrameCategory;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_6;
    QLabel *cLabel;
    QComboBox *categoryComboBox;
    QSpacerItem *horizontalSpacer_7;
    QWidget *contentWidgetCustom;
    QTreeView *treeViewCustom;
    QWidget *sideWidgetCustom;
    QToolButton *editButton_2;
    QToolButton *presetButton;
    QWidget *bottomWidgetCustom;
    QPushButton *pushButton_2;
    QWidget *modeBarWidget;
    QRadioButton *radioBasic;
    QRadioButton *radioCustom;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(399, 552);
        MainWindow->setMinimumSize(QSize(399, 552));
        MainWindow->setBaseSize(QSize(399, 520));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        modeWidget = new QWidget(centralWidget);
        modeWidget->setObjectName(QString::fromUtf8("modeWidget"));
        modeWidget->setGeometry(QRect(1, 1, 390, 521));
        sizePolicy.setHeightForWidth(modeWidget->sizePolicy().hasHeightForWidth());
        modeWidget->setSizePolicy(sizePolicy);
        modeWidget->setMinimumSize(QSize(0, 0));
        stackedWidget = new QStackedWidget(modeWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 50, 389, 477));
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        stackedWidget->setMinimumSize(QSize(0, 471));
        pageBasic = new QWidget();
        pageBasic->setObjectName(QString::fromUtf8("pageBasic"));
        sizePolicy.setHeightForWidth(pageBasic->sizePolicy().hasHeightForWidth());
        pageBasic->setSizePolicy(sizePolicy);
        bottomWidgetBasic = new QWidget(pageBasic);
        bottomWidgetBasic->setObjectName(QString::fromUtf8("bottomWidgetBasic"));
        bottomWidgetBasic->setGeometry(QRect(3, 410, 391, 61));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(bottomWidgetBasic->sizePolicy().hasHeightForWidth());
        bottomWidgetBasic->setSizePolicy(sizePolicy1);
        bottomWidgetBasic->setMinimumSize(QSize(391, 61));
        pushButton = new QPushButton(bottomWidgetBasic);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(109, 12, 121, 31));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy2);
        pushButton->setMinimumSize(QSize(121, 31));
        contentWidgetBasic = new QWidget(pageBasic);
        contentWidgetBasic->setObjectName(QString::fromUtf8("contentWidgetBasic"));
        contentWidgetBasic->setGeometry(QRect(3, 107, 391, 296));
        sizePolicy.setHeightForWidth(contentWidgetBasic->sizePolicy().hasHeightForWidth());
        contentWidgetBasic->setSizePolicy(sizePolicy);
        contentWidgetBasic->setMinimumSize(QSize(0, 296));
        treeViewBasic = new QTreeView(contentWidgetBasic);
        treeViewBasic->setObjectName(QString::fromUtf8("treeViewBasic"));
        treeViewBasic->setGeometry(QRect(22, 1, 289, 301));
        treeViewBasic->setMinimumSize(QSize(289, 301));
        treeViewBasic->header()->setDefaultSectionSize(0);
        treeViewBasic->header()->setProperty("showSortIndicator", QVariant(false));
        sideWidgetBasic = new QWidget(contentWidgetBasic);
        sideWidgetBasic->setObjectName(QString::fromUtf8("sideWidgetBasic"));
        sideWidgetBasic->setGeometry(QRect(318, 1, 70, 301));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(sideWidgetBasic->sizePolicy().hasHeightForWidth());
        sideWidgetBasic->setSizePolicy(sizePolicy3);
        sideWidgetBasic->setMinimumSize(QSize(70, 301));
        editButton = new QToolButton(sideWidgetBasic);
        editButton->setObjectName(QString::fromUtf8("editButton"));
        editButton->setGeometry(QRect(12, 2, 50, 50));
        sizePolicy2.setHeightForWidth(editButton->sizePolicy().hasHeightForWidth());
        editButton->setSizePolicy(sizePolicy2);
        editButton->setMinimumSize(QSize(50, 50));
        editButton->setMaximumSize(QSize(50, 50));
        searchWidget = new QWidget(pageBasic);
        searchWidget->setObjectName(QString::fromUtf8("searchWidget"));
        searchWidget->setGeometry(QRect(3, 59, 391, 41));
        sizePolicy1.setHeightForWidth(searchWidget->sizePolicy().hasHeightForWidth());
        searchWidget->setSizePolicy(sizePolicy1);
        searchWidget->setMinimumSize(QSize(0, 41));
        lineEdit = new QLineEdit(searchWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(22, 12, 161, 22));
        topFrameRandD = new QFrame(pageBasic);
        topFrameRandD->setObjectName(QString::fromUtf8("topFrameRandD"));
        topFrameRandD->setGeometry(QRect(3, 2, 391, 50));
        sizePolicy1.setHeightForWidth(topFrameRandD->sizePolicy().hasHeightForWidth());
        topFrameRandD->setSizePolicy(sizePolicy1);
        topFrameRandD->setMinimumSize(QSize(0, 50));
        topFrameRandD->setLayoutDirection(Qt::LeftToRight);
        topFrameRandD->setFrameShape(QFrame::StyledPanel);
        topFrameRandD->setFrameShadow(QFrame::Raised);
        rLabel = new QLabel(topFrameRandD);
        rLabel->setObjectName(QString::fromUtf8("rLabel"));
        rLabel->setGeometry(QRect(15, 14, 37, 18));
        rComboBox = new QComboBox(topFrameRandD);
        rComboBox->setObjectName(QString::fromUtf8("rComboBox"));
        rComboBox->setGeometry(QRect(59, 14, 120, 22));
        sizePolicy3.setHeightForWidth(rComboBox->sizePolicy().hasHeightForWidth());
        rComboBox->setSizePolicy(sizePolicy3);
        rComboBox->setMinimumSize(QSize(120, 0));
        dLabel = new QLabel(topFrameRandD);
        dLabel->setObjectName(QString::fromUtf8("dLabel"));
        dLabel->setGeometry(QRect(208, 14, 55, 18));
        dComboBox = new QComboBox(topFrameRandD);
        dComboBox->setObjectName(QString::fromUtf8("dComboBox"));
        dComboBox->setGeometry(QRect(270, 14, 79, 22));
        sizePolicy3.setHeightForWidth(dComboBox->sizePolicy().hasHeightForWidth());
        dComboBox->setSizePolicy(sizePolicy3);
        stackedWidget->addWidget(pageBasic);
        pageCustom = new QWidget();
        pageCustom->setObjectName(QString::fromUtf8("pageCustom"));
        topFrameCategory = new QFrame(pageCustom);
        topFrameCategory->setObjectName(QString::fromUtf8("topFrameCategory"));
        topFrameCategory->setGeometry(QRect(0, 0, 391, 61));
        sizePolicy1.setHeightForWidth(topFrameCategory->sizePolicy().hasHeightForWidth());
        topFrameCategory->setSizePolicy(sizePolicy1);
        topFrameCategory->setMinimumSize(QSize(0, 50));
        topFrameCategory->setLayoutDirection(Qt::LeftToRight);
        topFrameCategory->setFrameShape(QFrame::StyledPanel);
        topFrameCategory->setFrameShadow(QFrame::Raised);
        layoutWidget = new QWidget(topFrameCategory);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 20, 391, 24));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_6 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        cLabel = new QLabel(layoutWidget);
        cLabel->setObjectName(QString::fromUtf8("cLabel"));

        horizontalLayout->addWidget(cLabel);

        categoryComboBox = new QComboBox(layoutWidget);
        categoryComboBox->setObjectName(QString::fromUtf8("categoryComboBox"));
        categoryComboBox->setMinimumSize(QSize(220, 0));

        horizontalLayout->addWidget(categoryComboBox);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);

        contentWidgetCustom = new QWidget(pageCustom);
        contentWidgetCustom->setObjectName(QString::fromUtf8("contentWidgetCustom"));
        contentWidgetCustom->setGeometry(QRect(0, 60, 391, 351));
        treeViewCustom = new QTreeView(contentWidgetCustom);
        treeViewCustom->setObjectName(QString::fromUtf8("treeViewCustom"));
        treeViewCustom->setGeometry(QRect(20, 20, 301, 321));
        treeViewCustom->setMinimumSize(QSize(289, 301));
        treeViewCustom->header()->setDefaultSectionSize(0);
        treeViewCustom->header()->setProperty("showSortIndicator", QVariant(false));
        sideWidgetCustom = new QWidget(contentWidgetCustom);
        sideWidgetCustom->setObjectName(QString::fromUtf8("sideWidgetCustom"));
        sideWidgetCustom->setGeometry(QRect(320, 20, 70, 331));
        sizePolicy3.setHeightForWidth(sideWidgetCustom->sizePolicy().hasHeightForWidth());
        sideWidgetCustom->setSizePolicy(sizePolicy3);
        sideWidgetCustom->setMinimumSize(QSize(70, 301));
        editButton_2 = new QToolButton(sideWidgetCustom);
        editButton_2->setObjectName(QString::fromUtf8("editButton_2"));
        editButton_2->setGeometry(QRect(12, 2, 50, 50));
        sizePolicy2.setHeightForWidth(editButton_2->sizePolicy().hasHeightForWidth());
        editButton_2->setSizePolicy(sizePolicy2);
        editButton_2->setMinimumSize(QSize(50, 50));
        editButton_2->setMaximumSize(QSize(50, 50));
        presetButton = new QToolButton(sideWidgetCustom);
        presetButton->setObjectName(QString::fromUtf8("presetButton"));
        presetButton->setGeometry(QRect(12, 59, 50, 50));
        sizePolicy2.setHeightForWidth(presetButton->sizePolicy().hasHeightForWidth());
        presetButton->setSizePolicy(sizePolicy2);
        presetButton->setMinimumSize(QSize(50, 50));
        presetButton->setMaximumSize(QSize(50, 50));
        bottomWidgetCustom = new QWidget(pageCustom);
        bottomWidgetCustom->setObjectName(QString::fromUtf8("bottomWidgetCustom"));
        bottomWidgetCustom->setGeometry(QRect(0, 410, 391, 61));
        sizePolicy1.setHeightForWidth(bottomWidgetCustom->sizePolicy().hasHeightForWidth());
        bottomWidgetCustom->setSizePolicy(sizePolicy1);
        bottomWidgetCustom->setMinimumSize(QSize(391, 61));
        pushButton_2 = new QPushButton(bottomWidgetCustom);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(109, 12, 121, 31));
        sizePolicy2.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy2);
        pushButton_2->setMinimumSize(QSize(121, 31));
        stackedWidget->addWidget(pageCustom);
        modeBarWidget = new QWidget(modeWidget);
        modeBarWidget->setObjectName(QString::fromUtf8("modeBarWidget"));
        modeBarWidget->setGeometry(QRect(0, 1, 389, 51));
        sizePolicy1.setHeightForWidth(modeBarWidget->sizePolicy().hasHeightForWidth());
        modeBarWidget->setSizePolicy(sizePolicy1);
        modeBarWidget->setMinimumSize(QSize(0, 51));
        radioBasic = new QRadioButton(modeBarWidget);
        radioBasic->setObjectName(QString::fromUtf8("radioBasic"));
        radioBasic->setGeometry(QRect(62, 12, 121, 21));
        radioCustom = new QRadioButton(modeBarWidget);
        radioCustom->setObjectName(QString::fromUtf8("radioCustom"));
        radioCustom->setGeometry(QRect(190, 12, 134, 21));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "SE2", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\277\321\203\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        editButton->setText(QApplication::translate("MainWindow", "edit", 0, QApplication::UnicodeUTF8));
        lineEdit->setPlaceholderText(QApplication::translate("MainWindow", "\320\235\320\260\320\271\321\202\320\270 \321\201\320\272\321\200\320\270\320\277\321\202....", 0, QApplication::UnicodeUTF8));
        rLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.5pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt;\">\320\240\320\276\320\273\321\214:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        rComboBox->clear();
        rComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "\320\240\320\260\320\267\321\200\320\260\320\261\320\276\321\202\321\207\320\270\320\272", 0, QApplication::UnicodeUTF8)
        );
        dLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.5pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt;\">\320\237\321\200\320\270\320\261\320\276\321\200:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        dComboBox->clear();
        dComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "PLS06", 0, QApplication::UnicodeUTF8)
        );
        cLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.5pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt;\">\320\232\320\260\321\202\320\265\320\263\320\276\321\200\320\270\321\217:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        categoryComboBox->clear();
        categoryComboBox->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("MainWindow", "PLS26", 0, QApplication::UnicodeUTF8)
        );
        editButton_2->setText(QApplication::translate("MainWindow", "edit", 0, QApplication::UnicodeUTF8));
        presetButton->setText(QApplication::translate("MainWindow", "preset", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\277\321\203\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        radioBasic->setText(QApplication::translate("MainWindow", "\320\221\320\260\320\267\320\276\320\262\321\213\320\271 \321\200\320\265\320\266\320\270\320\274", 0, QApplication::UnicodeUTF8));
        radioCustom->setText(QApplication::translate("MainWindow", "\320\232\320\260\321\201\321\202\320\276\320\274\320\275\321\213\320\271 \321\200\320\265\320\266\320\270\320\274", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
