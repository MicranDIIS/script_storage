#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTreeWidgetItem>

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
    void buildLayouts();
    void loadScripts();
    void openSelectedScript();

private slots:
    void showBasicPage();
    void showCustomPage();
    void handleScriptDoubleClick(QTreeWidgetItem *item, int column);
};

#endif // MAINWINDOW_H
