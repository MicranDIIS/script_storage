#ifndef SE2_H
#define SE2_H
#include <QMainWindow>
#include <QMessageBox>
#include <QListWidget>
#include <QWidget>
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include <QtPlugin>
#include <QPluginLoader>
#include <QKeyEvent>
#include <QDebug>
#include <QToolBar>
#include <QLayout>

#include "interfaces.h"
#include "scriptsmanager.h"


namespace Ui {
    class SE3;
}



class SE2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit SE2(bool isStorageEditable, QWidget *parent = 0);
    ~SE2();
    void RunScriptFromConsole(QString path);

signals:
    void reOpenScript(const QModelIndex&);

protected slots:
    void handleSelectionChanged(const QItemSelection &selection);

private slots:
    void ScriptClosed();
    void ReloadScript();
    void UpdateLists();

    void on_tbNewScript_clicked();
    void on_tbEditScript_clicked();
    void on_tbDelScript_clicked();
    void on_tbRescan_clicked();
    void on_tbLaunch_clicked();
    void on_tbNetwork_toggled(bool checked);
    void on_tbDir_toggled(bool checked);
    void on_tbFind_toggled(bool checked);
    void on_leTitleFilter_textChanged(const QString &arg1);
    void on_leTextFilter_textChanged(const QString &arg1);
    void on_lvOperations_doubleClicked(const QModelIndex &index);

private:
    QVector<QMap<QString,QString> >scriptsList;
    QMap<QString,QString> filteredList;
    QMap<QString,int> relationMapSorted;
    Ui::SE3 *ui;
    QString scriptsPath;
    QPluginLoader *pl_obj;
    PluginInterface *pi_obj;

    bool bReloadScript;
    bool isRunFromConsole_;
    bool isEditScr;

    bool LoadScript(QString path, QString header);
    bool eventFilter(QObject *ev, QEvent *obj);
    void UpdateViewsCurrentIndex();
    void UpdateFileCnt(int count);

    int mCurrCategoryRow;
    int mCurrScriptRow;
    int m_files_cnt;
    bool m_editStorage;
    QList<stScriptInfo> m_scriptInfo;
    ScriptsManager scriptManag;
};

#endif // SE2_H
