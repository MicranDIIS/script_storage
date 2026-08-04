#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "diffeditor.h"

#include <QWidget>
#include <QSize>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>

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
//        QRect rect(0, 0, 40, 40);

//        painter.drawText(rect, Qt::AlignCenter | Qt::AlignTop, "1");

//        int top = blockBoundingGeometry(block)
//                      .translated(contentOffset())
//                      .top();

//        int y = 20 - diffEditor->verticalScrollBar()->value() * 2;
//        for (int i = 0; i <= this->height(); i++)
//        {
//            painter.drawText(20, y, QString::number(i+1));
//            y += 20;
//        }

//        QTextBlock block = diffEditor->firstVisibleBlock();
////        int top = blockBoundingGeometry(block).translated(contentOffset()).top();
//        painter.drawText(20, 20, block.blockNumber()+1);

        diffEditor->lineNumberAreaPaintEvent(event);
    }

private:
    DiffEditor *diffEditor;
};

#endif // LINENUMBERAREA_H
