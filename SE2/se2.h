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

namespace Ui {
    class SE2;
}

class SE2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit SE2(QWidget *parent = 0);    
    ~SE2();
    bool getScriptHeader(QString path, QString &specific, QString &category);
    void setSpecific(QString spec) { isRunFromConsole_ = true; specific_ = spec; }
    void setCategory(QString cat) { isRunFromConsole_ = true; category_ = cat; }
    void setScriptPath(QString path) { isRunFromConsole_ = true; scriptPath_ = path; }
    QObject* getObject(QString);

public slots:
    void runScript();

signals:
    void reOpenScript(QListWidgetItem*);

private slots:
    void checkClosingBaseWin();
    void myshow();
    void ReloadScript();
    bool eventFilter(QObject *ev, QEvent *obj);
    void on_cbCategories_currentIndexChanged(int index);
    void on_lwOperations_itemDoubleClicked(QListWidgetItem *item);
    void on_pbLaunch_clicked();
    bool loadScript(QString path, QString header);
    void on_tbNewScript_clicked();
    void on_tbEditScript_clicked();
    void on_tbDelScript_clicked();
    void updateLists();

private:
    QWidget* w;
    QVector< QMap<QString,QString> >ScriptsList;
    Ui::SE2 *ui;
    QObject *pl;
    QString path;
    QString specific_;
    QString category_;
    QString scriptPath_;
    bool bReloadScript;
    bool isRunFromConsole_;
    bool isNewScr;
    bool isEditScr;
    void FindScripts(QString);
    void recodeFile(QString path);
};

#endif // SE2_H
