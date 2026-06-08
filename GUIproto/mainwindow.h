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
#include <QSettings>
#include <commitinfo.h>

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

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    ViewModel *basicScriptsModel;
    ViewModel *customScriptsModel;

    ScriptFilterModel *basicFilterModel;
    ScriptFilterModel *customFilterModel;

    ComboFilterModel *deviceComboModel;
    ComboFilterModel *roleComboModel;
    ComboFilterModel *stadeComboModel;
    ComboFilterModel *categoryComboModel;

    void setupModels();
    void setupPageConnect();
    void setupFilterConnect();
    void setupActionConnect();
    void loadScripts();
    void openDiffForIndex(const QModelIndex &index);
    void resetFilterState();
    bool syncRepo();
    void loadState();
    void saveSettings();
    void openHistoryForIndex(const QModelIndex &index);
    QHash<QString, QPointer<HistoryWindow> > m_historyWindows;

    IRepository* m_repo;
    QString m_repoRoot;
    QSettings settings;

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
