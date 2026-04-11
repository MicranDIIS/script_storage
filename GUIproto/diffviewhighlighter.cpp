#include "diffviewhighlighter.h"

DiffViewHighlighter::DiffViewHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent)
{
    linesFormat.setBackground(QColor(000, 255, 000));
}

void DiffViewHighlighter::setHighlightedLines(const QSet<int> &lines)
{
    highlightedLines = lines;
    rehighlight();
}

void DiffViewHighlighter::highlightBlock(const QString& text)
{
    int current = currentBlock().blockNumber();

    if (highlightedLines.contains(current))
    {
        setFormat(0, text.length(), linesFormat);
    }
}
