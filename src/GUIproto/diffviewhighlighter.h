#ifndef DIFFVIEWHIGHLIGHTER_H
#define DIFFVIEWHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QSet>

class DiffViewHighlighter : public QSyntaxHighlighter
{
public:
    explicit DiffViewHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString& text);

private:
    QTextCharFormat addFormat;
    QTextCharFormat delFormat;
    QTextCharFormat headerFormat;
};

#endif // DIFFVIEWHIGHLIGHTER_H
