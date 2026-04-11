#ifndef DIFFVIEWHIGHLIGHTER_H
#define DIFFVIEWHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QSet>

class DiffViewHighlighter : public QSyntaxHighlighter
{
public:
    DiffViewHighlighter(QTextDocument* parent);
    void setHighlightedLines(const QSet<int> &lines);

protected:
    void highlightBlock(const QString& text);

private:
    QSet<int> highlightedLines;
    QTextCharFormat linesFormat;
};

#endif // DIFFVIEWHIGHLIGHTER_H
