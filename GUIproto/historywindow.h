#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "commitinfo.h"

#include <QWidget>
#include <QVector>
#include <QStandardItemModel>
#include <QStringList>
#include <QSettings>
#include <QSortFilterProxyModel>

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
    QSettings settings;
    QSortFilterProxyModel* m_proxy;
    void onCommitRowClicked(const QModelIndex& index);
    void updateCommitMessagePanel(const QModelIndex &indexInRow);
    void clearCommitMessagePanel();
    QString getSummaryString(const QString& fullMessage) const;
    QString getBodyString(const QString& fullMessage) const;


private slots:
    void onSortChanged(int);
    void onCurrentRowChanged(const QModelIndex &current);

protected:
    void closeEvent(QCloseEvent *event);
    enum {
        RoleCommitHash = Qt::UserRole + 1,
        RoleDateTime = Qt::UserRole + 2,
        RoleCommitMessage = Qt::UserRole + 3
    };

};

#endif // HISTORYWINDOW_H
