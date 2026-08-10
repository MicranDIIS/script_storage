    #include "diffviewerwindow.h"
#include "ui_diffviewerwindow.h"
#include "diffviewerstructs.h"

#include <QFile>
#include <QTextStream>
#include <QSet>

DiffViewerWindow::DiffViewerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiffViewerWindow)
{
    ui->setupUi(this);
    diffViewHighlighter = new DiffViewHighlighter(ui->ScriptPlainTextEdit->document());
}

DiffViewerWindow::~DiffViewerWindow()
{
    delete ui;
}

void DiffViewerWindow::setFilePath(const QString& path)
{
    scriptPath = path;
    LoadFileText();
}

void DiffViewerWindow::LoadFileText()
{
//    QFile scriptFile(scriptPath);

//    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QTextStream in(&scriptFile);
//    QString content = in.readAll();
//    ui->ScriptPlainTextEdit->setPlainText(content);

//    QSet<int> testLines;
//    testLines << 1 << 3 << 5 << 6 << 7;
//    diffViewHighlighter->setHighlightedLines(testLines);

    DiffResult mock = makeMock();
    QString text = buildUnifiedText(mock);

    ui->currentCommitMetaLabel->setText(mock.newCommit.author + " " + mock.newCommit.date);
    ui->previousCommitMetaLabel->setText(mock.oldCommit.author + " " + mock.oldCommit.date);

    ui->fileLabel->setText(scriptPath);

    diffViewHighlighter = new DiffViewHighlighter(ui->ScriptPlainTextEdit->document());

    ui->ScriptPlainTextEdit->setPlainText(text);

    ui->ScriptPlainTextEdit->setDiffLines(mock.hunks.first().lines);
}

DiffResult DiffViewerWindow::makeMock()
{
    DiffResult result;

    DiffHunk hunk;
    hunk.oldStart = 1;
    hunk.oldLines = 7;
    hunk.newStart = 1;
    hunk.newLines = 8;

    hunk.header = "@@ -1,7 +1,8 @@";

    hunk.lines.clear();
    hunk.lines.append(DiffLine(Context, 1,  1, "# Config file"));
    hunk.lines.append(DiffLine(Del,     2, -1, "VERSION = 1"));
    hunk.lines.append(DiffLine(Add,    -1,  2, "VERSION = 2"));
    hunk.lines.append(DiffLine(Del,      3, -1, "ENABLE_LOG = false"));
    hunk.lines.append(DiffLine(Add,    -1,  3, "ENABLE_LOG = true"));
    hunk.lines.append(DiffLine(Context, 4,  4, "TIMEOUT = 30"));
    hunk.lines.append(DiffLine(Add,    -1,  5, "RETRY = 3"));
    hunk.lines.append(DiffLine(Context, 5,  6, "PATH = /usr/bin"));
    hunk.lines.append(DiffLine(Del,     6, -1, "MODE = basic"));
    hunk.lines.append(DiffLine(Add,    -1,  7, "MODE = advanced"));
    hunk.lines.append(DiffLine(Context, 7,  8, "END"));
    for (int i = 9; i <= 50; ++i)
    {
        hunk.lines.append(DiffLine(Context, i, i,
            QString("Line number %1").arg(i)));
    }
    hunk.lines.append(DiffLine(Context, 1,  1, "# Config file"));
    hunk.lines.append(DiffLine(Del,     2, -1, "VERSION = 1"));
    hunk.lines.append(DiffLine(Add,    -1,  2, "VERSION = 2"));
    hunk.lines.append(DiffLine(Del,      3, -1, "ENABLE_LOG = false"));

    result.hunks.append(hunk);

    result.oldCommit.author = "Azaz232";
    result.oldCommit.date = "29/05/2026";

    result.newCommit.author = "Azaz232";
    result.newCommit.date = "30/07/2026";

    return result;
}

QString DiffViewerWindow::buildUnifiedText(const DiffResult& result)
{
    QStringList output;

    int size = 0;
    for (int i = 0; i < result.hunks.size(); ++i)
    {
        size += 1 + result.hunks[i].lines.size();
    }

    output.reserve(size);

    for (int i = 0; i < result.hunks.size(); ++i)
    {
         const DiffHunk& hunk = result.hunks[i];

         output.append(hunk.header);

         for (int j = 0; j < hunk.lines.size(); ++j)
         {
             const DiffLine& line = hunk.lines[j];

             QString prefix;

             if (line.type == Context)
                 prefix = "  ";
             else if (line.type == Del)
                 prefix = "- ";
             else if (line.type == Add)
                 prefix = "+ ";

             output.append(prefix + line.text);
         }
    }
    return output.join("\n");
}

