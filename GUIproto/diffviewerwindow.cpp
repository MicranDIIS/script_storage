#include "diffviewerwindow.h"
#include "ui_diffviewerwindow.h"

#include <QFile>
#include <QTextStream>

DiffViewerWindow::DiffViewerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiffViewerWindow)
{
    ui->setupUi(this);
}

DiffViewerWindow::~DiffViewerWindow()
{
    delete ui;
}

void DiffViewerWindow::setFilePath(const QString& path)
{
    scriptPath = path;
    LoadFileText();
}

void DiffViewerWindow::LoadFileText()
{
    QFile scriptFile(scriptPath);

    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&scriptFile);
    QString content = in.readAll();
    ui->ScriptPlainTextEdit->setPlainText(content);
}
