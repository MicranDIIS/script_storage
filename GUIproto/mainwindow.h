#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QModelIndex>
#include <QComboBox>
#include "scriptfiltermodel.h"
#include "combofiltermodel.h"

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
    ScriptFilterModel *filterModel;
    ComboFilterModel *deviceComboModel;
    ComboFilterModel *roleComboModel;
    ComboFilterModel *stadeComboModel;
    ComboFilterModel *categoryComboModel;

    void buildLayouts();
    void loadScripts();
    void resetComboBoxes();
    void resetFilterState();

private slots:
    void showBasicPage();
    void showCustomPage();
    void handleScriptDoubleClick(const QModelIndex &index);
    void openSelectedScript();
    void applyStadeFilter();
    void applyDeviceFilter();
    void applyRoleFilter();
    void applyCategoryFilter();
    void applyTextSearch(const QString& text);
};

#endif // MAINWINDOW_H
