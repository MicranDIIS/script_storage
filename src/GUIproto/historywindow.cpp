#include "historywindow.h"
#include "ui_historywindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QClipboard>
#include <QApplication>
#include <QTimer>


HistoryWindow::HistoryWindow(const QString& filePath,
                             const QVector<GuiCommitInfo>& history,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryWindow),
    m_historyModel(new QStandardItemModel(this)),
    m_proxy(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);

    setupModels();
    setupView();
    restoreUiGeometry();

    connect(ui->HistoryTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this,SLOT(onCurrentRowChanged(QModelIndex)));
    connect(ui->HistoryTableView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(copyHashToClipboard(QModelIndex)));

    updateData(filePath, history);
}

void HistoryWindow::setupModels()
{
    m_historyModel->setColumnCount(ColumnCount);
    m_headers.clear();
    m_headers << tr("Date")
              << tr("Author")
              << tr("Commit message");
    m_historyModel->setHorizontalHeaderLabels(m_headers);

    m_proxy->setSourceModel(m_historyModel);
    m_proxy->setSortRole(RoleDateTime);
    m_proxy->setDynamicSortFilter(true);
}

void HistoryWindow::setupView()
{
    ui->HistoryTableView->setModel(m_proxy);

    ui->HistoryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->HistoryTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->HistoryTableView->setSortingEnabled(true);
//    ui->HistoryTableView->sortByColumn(DateColumn, Qt::DescendingOrder);
}


void HistoryWindow::restoreUiGeometry()
{
    restoreGeometry(settings.value("HistoryWindow/Geometry").toByteArray());
    const   QByteArray state = settings.value("HistoryTableView/State").toByteArray();
    if (!state.isEmpty())
           ui->HistoryTableView->horizontalHeader()->restoreState(state);
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::setFilePath(const QString& filePath)
{
    m_scriptPath = filePath;
    ui->ScriptNameLabel->setText(QFileInfo(filePath).fileName());
    ui->ScriptNameLabel->setToolTip(m_scriptPath);
}

void HistoryWindow::setHistory(const QVector<GuiCommitInfo>& history)
{
    m_commitInfo = history;
    loadHistory();
}

void HistoryWindow::updateData(const QString& filePath,
                               const QVector<GuiCommitInfo>& history)
{
    setFilePath(filePath);
    setHistory(history);
}

void HistoryWindow::loadHistory()
{
    m_historyModel->setRowCount(0);

    foreach (const GuiCommitInfo& c, m_commitInfo)
    {
        QString dateStr = c.dateTime.toString("dd.MM.yyyy HH:mm");

        QStandardItem *dateItem = new QStandardItem(dateStr);
        QStandardItem *authorItem = new QStandardItem(c.author);

        QString summary = getSummaryString(c.commitMessage);
        QStandardItem *msgItem = new QStandardItem(summary);

        if (summary.isEmpty())
        {
            msgItem->setForeground(QBrush(Qt::gray));
        }

        dateItem->setData(c.commitHash, RoleCommitHash);
        dateItem->setData(c.dateTime, RoleDateTime);
        msgItem->setData(c.commitMessage, RoleCommitMessage);

        QList<QStandardItem*> row;
        row << dateItem << authorItem << msgItem;

        m_historyModel->appendRow(row);
    }
    m_proxy->sort(DateColumn, m_proxy->sortOrder());
}


void HistoryWindow::saveUiState()
{
    settings.setValue("HistoryWindow/Geometry", saveGeometry());

    QByteArray tableState = ui->HistoryTableView->horizontalHeader()->saveState();
    settings.setValue("HistoryTableView/State", tableState);
}


void HistoryWindow::closeEvent(QCloseEvent *event)
{
    saveUiState();
    QWidget::closeEvent(event);
}

void HistoryWindow::onCurrentRowChanged(const QModelIndex& current)
{
    if (!current.isValid())
    {
        clearCommitMessagePanel();
        return;
    }

    updateCommitMessagePanel(current);
}

void HistoryWindow::updateCommitMessagePanel(const QModelIndex &indexInRow)
{
    if (!indexInRow.isValid())
    {
        clearCommitMessagePanel();
        return;
    }

    QModelIndex messageIndex = indexInRow.sibling(indexInRow.row(), CommitColumn);

    if (!messageIndex.isValid())
    {
        clearCommitMessagePanel();
        return;
    }

//    QString summary = getSummaryString(messageIndex.data(RoleCommitMessage).toString());
    QString summary = messageIndex.data(Qt::DisplayRole).toString();
    if (summary.isEmpty())
    {
        summary = tr("Commit message was empty(");
    }

    QString body = getBodyString(messageIndex.data(RoleCommitMessage).toString());

    ui->CommitMessageTextEdit->clear();

    QTextCursor cursor(ui->CommitMessageTextEdit->document());
    cursor.movePosition(QTextCursor::Start);

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    cursor.insertText(summary, boldFormat);

    QTextCharFormat normalFormat;
    normalFormat.setFontWeight(QFont::Normal);

    if (!body.isEmpty()) {
        cursor.insertBlock();
        cursor.insertBlock();
        cursor.insertText(body, normalFormat);
    }
}

void HistoryWindow::clearCommitMessagePanel()
{
    ui->CommitMessageTextEdit->clear();
}

QStringList HistoryWindow::splitString(const QString& fullMessage)  const
{
    QString string = fullMessage;
    string.replace("\r\n", "\n");
    string.replace("\r", "\n");
    return string.split("\n");
}

QString HistoryWindow::getSummaryString(const QString& fullMessage) const
{
    QStringList lines = splitString(fullMessage);

    for (int i = 0; i < lines.size(); i++)
    {
        QString line = lines[i].trimmed();

        if (!line.isEmpty())
        {
            return line;
        }
    }
    return "";
}

QString HistoryWindow::getBodyString(const QString& fullMessage) const
{
    QStringList lines = splitString(fullMessage);

    int summaryIndex = -1;
    for (int i = 0; i < lines.size(); ++i)
    {
        if (!lines.at(i).trimmed().isEmpty())
        {
            summaryIndex = i;
            break;
        }
    }

    if (summaryIndex == -1)
        return QString();

    QStringList bodyLines;
    for (int i = summaryIndex + 1; i < lines.size(); ++i)
        bodyLines << lines.at(i);

    while (!bodyLines.isEmpty() && bodyLines.at(0).trimmed().isEmpty())
        bodyLines.removeAt(0);

    return bodyLines.join("\n");
}

void HistoryWindow::copyHashToClipboard(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QString hash = index.sibling(index.row(), DateColumn).data(RoleCommitHash).toString();

    if (hash.isEmpty())
    {
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(hash);
//    QToolTip::showText(QCursor::pos(), trUtf8("Хэш скопирован"));
    ui->hashLabel->setText(tr("Hash copied!"));
    ui->hashLabel->show();
    QTimer::singleShot(2000, ui->hashLabel, SLOT(hide()));
}
