#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include <QMainWindow>
#include <interfaces.h>

namespace Ui {
class ScriptWindow;
}

class ScriptWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptWindow(QWidget *parent = 0);
    ~ScriptWindow();

    void openScriptUI(const QString &path, const QString &baseWinPath);

private:
    Ui::ScriptWindow *ui;
    QObject *pluginObject;
};

#endif // SCRIPTWINDOW_H
