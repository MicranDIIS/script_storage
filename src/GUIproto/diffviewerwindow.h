#ifndef DIFFVIEWERWINDOW_H
#define DIFFVIEWERWINDOW_H

#include "diffviewhighlighter.h"

#include <QWidget>
#include <QFile>
#include <QTextStream>

namespace Ui {
    class DiffViewerWindow;
}

class DiffViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DiffViewerWindow(QWidget *parent = 0);
    ~DiffViewerWindow();

    void setFilePath(const QString& path);

private:
    Ui::DiffViewerWindow *ui;
    QString scriptPath;
    void LoadFileText();
    DiffViewHighlighter * diffViewHighlighter;
};

#endif // DIFFVIEWERWINDOW_H
