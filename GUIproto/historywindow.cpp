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

    ui->HistoryTableView->setModel(m_historyModel);

    m_historyModel->setColumnCount(3);
    m_headers << QString::fromLocal8Bit("Дата")
              << QString::fromLocal8Bit("Автор")
              << QString::fromLocal8Bit("Сообщение коммита");
    m_historyModel->setHorizontalHeaderLabels(m_headers);

    QByteArray state = settings.value("HistoryTableView/State").toByteArray();
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
        QStandardItem *msgItem = new QStandardItem(c.commitMessage);

        dateItem->setData(c.commitHash, Qt::UserRole);

        QList<QStandardItem*> row;
        row << dateItem << authorItem << msgItem;

        m_historyModel->appendRow(row);
    }

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

    return mockHistory;
}

void HistoryWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("HistoryWindow/Geometry", saveGeometry());

    QByteArray tableState = ui->HistoryTableView->horizontalHeader()->saveState();
    settings.setValue("HistoryTableView/State", tableState);

    QWidget::closeEvent(event);
}
