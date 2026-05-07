#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QModelIndex>
#include <QComboBox>
#include "scriptfiltermodel.h"
#include "combofiltermodel.h"
#include <QHash>
#include <QPointer>
#include <QPoint>
//#include "historywindow.h"
class HistoryWindow;

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
    void openDiffForIndex(const QModelIndex &index);
    void resetComboBoxes();
    void resetFilterState();
    void openHistoryForIndex(const QModelIndex &index);
    QHash<QString, QPointer<HistoryWindow> > m_historyWindows;


private slots:
    void showBasicPage();
    void showCustomPage();
    void handleScriptDoubleClick(const QModelIndex &index);
    void openSelectedScript();
//    void showContextMenu(const QPoint& pos);
    void applyStadeFilter();
    void applyDeviceFilter();
    void applyRoleFilter();
    void applyCategoryFilter();
    void applyTextSearch(const QString& text);
    void showCustomContextMenu(const QPoint& pos);
    void showBasicContextMenu(const QPoint &pos);
    void onHistoryWindowDestroyed(QObject* obj);
};

#endif // MAINWINDOW_H
