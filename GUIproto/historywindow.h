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
    void setHistory(const QVector<GuiCommitInfo>& history);

private:
    Ui::HistoryWindow *ui;
    QString m_scriptPath;
    QVector<GuiCommitInfo> m_commitInfo;
    QStandardItemModel* m_historyModel;
    void loadHistory();
    QStringList m_headers;
    QVector<GuiCommitInfo> makeMockHistory() const;
    QSettings settings;
    QSortFilterProxyModel* m_proxy;
    void updateCommitMessagePanel(const QModelIndex &indexInRow);
    void clearCommitMessagePanel();
    QString getSummaryString(const QString& fullMessage) const;
    QString getBodyString(const QString& fullMessage) const;
    QStringList splitString(const QString& fullMessage) const;
    enum Column
    {
        DateColumn = 0,
        AuthorColumn = 1,
        CommitColumn = 2,
        ColumnCount = 3
    };
    void setupModels();


private slots:
    void onCurrentRowChanged(const QModelIndex &current);
    void copyHashToClipboard(const QModelIndex& index);

protected:
    void closeEvent(QCloseEvent *event);
    enum
    {
        RoleCommitHash = Qt::UserRole + 1,
        RoleDateTime = Qt::UserRole + 2,
        RoleCommitMessage = Qt::UserRole + 3
    };

};

#endif // HISTORYWINDOW_H
