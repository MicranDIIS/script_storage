#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>

namespace Ui {
    class HistoryWindow;
}

class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWindow(QWidget *parent = 0);
    ~HistoryWindow();
    void setFilePath(const QString& scriptPath);

private:
    Ui::HistoryWindow *ui;
    QString scriptPath;
};

#endif // HISTORYWINDOW_H
