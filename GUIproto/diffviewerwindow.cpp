#include "diffviewerwindow.h"
#include "ui_diffviewerwindow.h"

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
