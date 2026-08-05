#include "diffeditor.h"

#include "linenumberarea.h"

#include <QDebug>

DiffEditor::DiffEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setViewportMargins(40, 0, 0, 0);

    lineNumberArea = new LineNumberArea(this);

//    connect(this, &DiffEditor::blockCountChanged, this, &DiffEditor::updateLineNumberAreaWidth);
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
}

void DiffEditor::setDiffLines(const QList<DiffLine> &lines)
{
    diffLines = lines;
    lineNumberArea->update();
}

void DiffEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberArea->setGeometry(0, 0, 40, this->height());
}

void DiffEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    Q_UNUSED(rect);
    Q_UNUSED(dy);
//    qDebug() << "update line numbers";

    lineNumberArea->update();
}

void DiffEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.setPen(QColor(Qt::black));

    QTextBlock block = this->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    int y = top + fontMetrics().ascent();

    int  oldX = 5;
    int newX = 30;

    while (block.isValid() && top <= this->height())
    {
//        painter.drawText(20, y, QString::number(blockNumber+1));
//        top += blockBoundingRect(block).height();
//        y = top + fontMetrics().ascent();
//        block = block.next();
//        ++blockNumber;
        if (blockNumber > 0 && blockNumber - 1 < diffLines.size())
        {
            int diffIndex = blockNumber - 1;

            if (diffIndex >= 0 && diffIndex < diffLines.size())
            {
                const DiffLine &line = diffLines[diffIndex];

                if (line.oldNum != -1)
                    painter.drawText(oldX, y, QString::number(line.oldNum));

                if (line.newNum != -1)
                    painter.drawText(newX, y, QString::number(line.newNum));
            }
        }
        top += blockBoundingRect(block).height();
        y = top + fontMetrics().ascent();
        block = block.next();
        ++blockNumber;
    }
}
