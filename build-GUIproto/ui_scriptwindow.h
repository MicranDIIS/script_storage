/********************************************************************************
** Form generated from reading UI file 'scriptwindow.ui'
**
** Created: Wed 1. Apr 14:33:50 2026
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCRIPTWINDOW_H
#define UI_SCRIPTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScriptWindow
{
public:
    QWidget *centralwidget;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ScriptWindow)
    {
        if (ScriptWindow->objectName().isEmpty())
            ScriptWindow->setObjectName(QString::fromUtf8("ScriptWindow"));
        ScriptWindow->resize(583, 416);
        centralwidget = new QWidget(ScriptWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 110, 351, 251));
        ScriptWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ScriptWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 583, 20));
        ScriptWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ScriptWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ScriptWindow->setStatusBar(statusbar);

        retranslateUi(ScriptWindow);

        QMetaObject::connectSlotsByName(ScriptWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ScriptWindow)
    {
        ScriptWindow->setWindowTitle(QApplication::translate("ScriptWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ScriptWindow: public Ui_ScriptWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCRIPTWINDOW_H
