#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "diffeditor.h"

#include <QWidget>
#include <QSize>
#include <QPainter>

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(DiffEditor *editor) : QWidget(editor), diffEditor(editor)
    {}

//    QSize sizeHint() const
//    {
//        return QSize(diffEditor->lineNumberAreaWidth(), 0);
//    }

protected:
    void paintEvent(QPaintEvent *event)
    {
//        diffEditor->lineNumberAreaPaintEvent(event);
        QPainter painter(this);
        QRect rect(0, 0, width(), height());
        painter.fillRect(rect, Qt::blue);

    }

private:
    DiffEditor *diffEditor;
};

#endif // LINENUMBERAREA_H
