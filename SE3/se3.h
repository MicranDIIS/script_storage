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

#include <QKeyEvent>
#include <QDebug>
#include <QToolBar>
#include <QLayout>


#include "scriptsmanager.h"
#include "scriptitemdelegate.h"
#include "../include/findmodules.h"
#include "../include/blockarea.h"
#include "../include/backuper.h"

namespace Ui {
    class SE3;
}

class SE3 : public QMainWindow, public ErrorManager
{
    Q_OBJECT

#ifdef TEST_MODE
    friend class SE3_Test;
#endif

public:
    explicit SE3(bool isStorageEditable=false, QWidget *parent = 0);
    ~SE3();
    void init();

public slots:
    void RunScriptFromConsole(QString path);

signals:
    void reOpenScript(const QModelIndex&);
    void reOpenScript(QString);

protected slots:
    void handleSelectionChanged(const QItemSelection &selection);
    void handleSelectionScriptsChanged(const QItemSelection &selection);

private slots:
    void ScriptClosed();
    void ReloadScript(QString path);
    void UpdateLists();
    void SaveNewFile(QString title, QString category, int lang);

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
    void on_tbCopyScript_clicked();
    void on_tbLinkScript_clicked();
    void on_tbUpdateScript_clicked();
    void on_tbRestoreScript_clicked();

private:
    QVector<QMap<QString,QString> >scriptsList;
    QMap<QString,QString> filteredList;
    QMap<QString,int> relationMapSorted;
    Ui::SE3 *ui;

    PluginInterface *pi_obj;
    QString lastScriptPath;

    bool bReloadScript;
    bool LoadScript(stScriptFileInfo info);

    void UpdateViewsCurrentIndex();
    void UpdateFileCnt(int count);
    void ShowLongMessage(QString text,bool show);
    void SaveCurrentScriptIndex();
    void RestoreCurrentScriptIndex();
    void CheckRestoreUpdateAvailable();
    bool eventFilter(QObject *ev, QEvent *obj);
    QString GetCurrScriptHead();
    QString GetCurrScriptPath();

    int mCurrCategoryRow;
    int mCurrScriptRow;
    int m_files_cnt;
    bool m_editStorage;
    QString m_app_path;

    QList<stScriptInfo> m_scriptInfo;
    ScriptsManager* scriptManag;
    CBlockArea* m_blockArea;
    stLoadFlags lFlags;
};

#endif // SE2_H
