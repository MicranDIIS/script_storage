#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStandardItemModel>
#include <QModelIndex>
#include "viewmodel.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ViewModel *scriptsModel;

    void buildLayouts();
    void loadScripts();

private slots:
    void showBasicPage();
    void showCustomPage();
    void handleScriptDoubleClick(const QModelIndex &index);
    void openSelectedScript();

};

#endif // MAINWINDOW_H
