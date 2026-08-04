#ifndef DIFFEDITOR_H
#define DIFFEDITOR_H

#include <QPlainTextEdit>

class DiffEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    DiffEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
//    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberArea(const QRect &rect, int dy);
//    void updateLineNumberWidth(int newBlockCount);

private:
    QWidget *lineNumberArea;
};

#endif // DIFFEDITOR_H
