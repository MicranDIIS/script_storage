#include "historywindow.h"
#include "ui_historywindow.h"

#include <QDebug>

HistoryWindow::HistoryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryWindow)
{
    ui->setupUi(this);
    qDebug("layout ptr = %p", this->layout());
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::setFilePath(const QString& filePath)
{
    scriptPath = filePath;
}
