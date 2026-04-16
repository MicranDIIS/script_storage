#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "commitinfo.h"

#include <QWidget>
#include <QVector>
#include <QStandardItemModel>
#include <QStringList>

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
    void setHistory(const QVector<CommitInfo>& history);

private:
    Ui::HistoryWindow *ui;
    QString m_scriptPath;
    QVector<CommitInfo> m_commitInfo;
    QStandardItemModel* m_historyModel;
    void loadHistory();
    QStringList m_headers;
    QVector<CommitInfo> makeMockHistory() const;

};

#endif // HISTORYWINDOW_H
