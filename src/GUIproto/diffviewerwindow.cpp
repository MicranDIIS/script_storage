#include "diffviewerwindow.h"
#include "ui_diffviewerwindow.h"
//#include "diffviewerstructs.h"
#include "mgit.h"

#include <QFile>
#include <QTextStream>
#include <QSet>

DiffViewerWindow::DiffViewerWindow(const DiffResult &result, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiffViewerWindow),
    m_result(result)
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

    if (m_result.hunks.isEmpty())
    {
        ui->ScriptPlainTextEdit->setPlainText(tr("No changes to display."));
        return;
    }
    QString text = buildUnifiedText(m_result);
    ui->ScriptPlainTextEdit->setPlainText(text);
    ui->ScriptPlainTextEdit->setDiffLines(m_result.hunks.first().lines);

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

         output.append(hunk.header.trimmed());

         for (int j = 0; j < hunk.lines.size(); ++j)
         {
             const DiffLine& line = hunk.lines[j];

             QString prefix;

             if (line.type == CONTEXT)
                 prefix = "  ";
             else if (line.type == DEL)
                 prefix = "- ";
             else if (line.type == ADD)
                 prefix = "+ ";

             output.append(prefix + line.text);
         }
    }
    return output.join("\n");
}

