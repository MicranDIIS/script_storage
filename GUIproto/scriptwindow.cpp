#include "scriptwindow.h"
#include "ui_scriptwindow.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPluginLoader>
#include <QMessageBox>
#include <interfaces.h>

ScriptWindow::ScriptWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScriptWindow)
{
    ui->setupUi(this);
}

ScriptWindow::~ScriptWindow()
{
    delete ui;
}

void ScriptWindow::openScriptUI(const QString &path, const QString &baseWinPath)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    if (!QFileInfo(baseWinPath).exists())

        return;
//    qDebug("it exists");


    QPluginLoader loader(baseWinPath);
    pluginObject = loader.instance();
    PluginInterface *pluginInterface = qobject_cast<PluginInterface *>(pluginObject);

    if (!pluginInterface){
      QMessageBox::critical(this,tr("Error BasePlugin.dll"),loader.errorString());
      return;
    }

    QTextStream in(&file);
    ui->plainTextEdit->setPlainText(in.readAll());

    setWindowTitle(QFileInfo(path).fileName());

//    bool result = true;
//    QDir dir(qApp->applicationDirPath());
//    dir.cd("plugins");
//    QString baseWinPath = QApplication::applicationDirPath() + "/plugins/BaseWin.dll";
//    QPluginLoader loader(baseWinPath);
//    pl = loader.instance();
//        PluginInterface *pi=qobject_cast<PluginInterface *>(pl);
//    if (!pi){
//	QMessageBox::critical(this,tr("Error BasePlugin.dll"),loader.errorString());
//        result = false;
//        return result;
//    }
//    pi->SetPath(path+";"+ui->cbCategories->currentText());
//    pi->startThread();
}

