#include "diffeditor.h"

#include "linenumberarea.h"

#include <QDebug>

DiffEditor::DiffEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidth));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
}

void DiffEditor::setDiffLines(const QList<DiffLine> &lines)
{
    diffLines = lines;
    lineNumberArea->update();
    updateLineNumberWidth();
}

int DiffEditor::lineNumberAreaWidth()
{
    int maxOld = 0;
    int maxNew = 0;

    for (int i = 0; i < diffLines.size(); ++i)
    {
        if (diffLines[i].oldNum > maxOld)
            maxOld = diffLines[i].oldNum;

        if (diffLines[i].newNum > maxNew)
            maxNew = diffLines[i].newNum;
    }

    QFontMetrics fm(font());

    int widthOld = fm.width(QString::number(maxOld));
    int widthNew = fm.width(QString::number(maxNew));

    int totalWidth = widthOld + widthNew + 20;

    return totalWidth;
}

void DiffEditor::updateLineNumberWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void DiffEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberArea->setGeometry(0, 0, lineNumberAreaWidth(), this->height());
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
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());

    int y = top + fontMetrics().ascent();

    int  oldX = 5;
    int newX = 30;

    while (block.isValid() && top <= this->height())
    {
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
//        y = top + fontMetrics().ascent() + 2;

        int baselineShift = fontMetrics().descent();
        y = top + fontMetrics().ascent() + baselineShift;

        block = block.next();
        ++blockNumber;
    }
}
