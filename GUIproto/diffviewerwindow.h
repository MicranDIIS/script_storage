#ifndef DIFFVIEWERWINDOW_H
#define DIFFVIEWERWINDOW_H

#include <QWidget>

namespace Ui {
    class DiffViewerWindow;
}

class DiffViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DiffViewerWindow(QWidget *parent = 0);
    ~DiffViewerWindow();

private:
    Ui::DiffViewerWindow *ui;
};

#endif // DIFFVIEWERWINDOW_H
