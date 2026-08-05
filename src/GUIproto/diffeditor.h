#ifndef DIFFEDITOR_H
#define DIFFEDITOR_H

#include "diffviewerstructs.h"

#include <QPlainTextEdit>

class DiffEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    DiffEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setDiffLines(const QList<DiffLine> &lines);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberArea(const QRect &rect, int dy);
    void updateLineNumberWidth();

private:
    QWidget *lineNumberArea;
    QList<DiffLine> diffLines;
};

#endif // DIFFEDITOR_H
