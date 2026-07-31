#include "diffviewhighlighter.h"

DiffViewHighlighter::DiffViewHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent)
{
    addFormat.setBackground(QColor(200, 255, 200));
    addFormat.setForeground(Qt::black);

    delFormat.setBackground(QColor(255, 200, 200));
    delFormat.setForeground(Qt::black);

    headerFormat.setBackground(QColor(220, 220, 220));
    headerFormat.setFontWeight(QFont::Bold);
}


void DiffViewHighlighter::highlightBlock(const QString& text)
{
    if (text.startsWith("+ "))
    {
        setFormat(0, text.length(), addFormat);
    }
    else if (text.startsWith("- "))
    {
        setFormat(0, text.length(), delFormat);
    }
    else if(text.startsWith("@@"))
    {
        setFormat(0, text.length(), headerFormat);
    }
}
