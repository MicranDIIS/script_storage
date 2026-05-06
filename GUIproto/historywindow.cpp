#include "historywindow.h"
#include "ui_historywindow.h"

#include <QDebug>
#include <QFileInfo>

HistoryWindow::HistoryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryWindow),
    m_historyModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

//    qDebug("layout ptr = %p", this->layout());

    restoreGeometry(settings.value("HistoryWindow/Geometry").toByteArray());

    m_proxy = new QSortFilterProxyModel(this);

    m_proxy->setSourceModel(m_historyModel);
    m_proxy->setSortRole(RoleDateTime);
    m_proxy->setDynamicSortFilter(true);

    ui->HistoryTableView->setModel(m_proxy);
    ui->HistoryTableView->setSortingEnabled(true);
    m_proxy->sort(0, Qt::DescendingOrder);

    m_historyModel->setColumnCount(3);
    m_headers << trUtf8("Дата")
              << trUtf8("Автор")
              << trUtf8("Сообщение коммита");
    m_historyModel->setHorizontalHeaderLabels(m_headers);

    QByteArray state = settings.value("HistoryTableView/State").toByteArray();
    ui->HistoryTableView->horizontalHeader()->restoreState(state);

//    connect(ui->FilterComboBox, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(onSortChanged(int)));

    ui->HistoryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->HistoryTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->HistoryTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this,SLOT(onCurrentRowChanged(QModelIndex)));

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

    setHistory(makeMockHistory());
}

void HistoryWindow::setHistory(const QVector<CommitInfo>& history)
{
    m_commitInfo = history;
    loadHistory();
}

void HistoryWindow::loadHistory()
{
    m_historyModel->setRowCount(0);

    foreach (const CommitInfo& c, m_commitInfo)
    {
        QString dateStr = c.dateTime.toString("dd.MM.yyyy HH:mm");

        QStandardItem *dateItem = new QStandardItem(dateStr);
        QStandardItem *authorItem = new QStandardItem(c.author);

        QStandardItem *msgItem = new QStandardItem(getSummaryString(c.commitMessage));

        if (c.commitMessage.trimmed().isEmpty())
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
    m_proxy->sort(0, m_proxy->sortOrder());
}

QVector<CommitInfo> HistoryWindow::makeMockHistory() const
{
    QVector<CommitInfo> mockHistory;

    CommitInfo a;
    a.dateTime = QDateTime::currentDateTime().addDays(-1);
    a.author = "Sonya";
    a.authorEmail = "sofasennikovablablabla@gmail.com";
    a.commitMessage = "blablablablablablablabla";
    a.commitHash = "a1b2c3d";
    mockHistory.append(a);

    CommitInfo b;
    b.dateTime = QDateTime::currentDateTime().addDays(-7);
    b.author = "Vanya";
    b.authorEmail = "kjhghkd@gmail.com";
    b.commitMessage = "Refactor header parsing (long message to test table resizing)";
    b.commitHash = "d4e5f6a";
    mockHistory.append(b);

    CommitInfo c;
    c.dateTime = QDateTime::currentDateTime().addDays(-3);
    c.author = "Vasya";
    c.authorEmail = "kd@gmail.com";
    c.commitMessage = "";
    c.commitHash = "d4e7y6a";
    mockHistory.append(c);

    CommitInfo d;
    d.dateTime = QDateTime::currentDateTime().addDays(-3);
    d.author = "Anton";
    d.authorEmail = "khrenkov@gmail.com";
    d.commitMessage = QString::fromUtf8("Добавил первичную прибавку к мощности генератора\n"
            "Определение и прибавка к мощности генератора осуществляется с\n"
            "целью более быстрого поиска необходимого уровня мощности.\n"
            "modified: scripts/sk4m/sk4m-50/PSI/50__9_9.lua");
    d.commitHash = "d7h7y6a";
    mockHistory.append(d);


    return mockHistory;
}

void HistoryWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("HistoryWindow/Geometry", saveGeometry());

    QByteArray tableState = ui->HistoryTableView->horizontalHeader()->saveState();
    settings.setValue("HistoryTableView/State", tableState);

    QWidget::closeEvent(event);
}

void HistoryWindow::onSortChanged(int index)
{
    Qt::SortOrder order = Qt::DescendingOrder;
    if (index == 1)
    {
        order = Qt::AscendingOrder;
    }
    m_proxy->sort(0, order);

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

    QModelIndex messageIndex = indexInRow.sibling(indexInRow.row(), 2);

    if (!messageIndex.isValid())
    {
        clearCommitMessagePanel();
        return;
    }

    QString summary = getSummaryString(messageIndex.data(RoleCommitMessage).toString());
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

QString HistoryWindow::getSummaryString(const QString& fullMessage) const
{
    QString string = fullMessage;

    string.replace("\r\n", "\n");
    string.replace("\r", "\n");

    QStringList lines = string.split("\n");

    for (int i = 0; i < lines.size(); i++)
    {
        QString line = lines[i].trimmed();

        if (!line.isEmpty())
        {
            return line;
        }
    }
    return trUtf8("Сообщение коммита было пустым(");
}

QString HistoryWindow::getBodyString(const QString& fullMessage) const
{
    QString string = fullMessage;

    string.replace("\r\n", "\n");
    string.replace("\r", "\n");

    QStringList lines = string.split("\n");

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
