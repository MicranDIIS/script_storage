#include "diffeditor.h"

#include "linenumberarea.h"

#include <QDebug>

DiffEditor::DiffEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
}

void DiffEditor::setDiffLines(const QList<DiffLine> &lines)
{
    diffLines = lines;
    changedStarts = findChangedStarts(lines);
    currentChangedIndex = -1;
    lineNumberArea->update();
    updateLineNumberWidth(0);
}

QList<int> DiffEditor::findChangedStarts(const QList<DiffLine> &lines)
{
    QList<int> changedStarts;
    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines[i].type != CONTEXT &&
            (i == 0 || lines[i - 1].type == CONTEXT))
        {
            qDebug() << i+1;
            changedStarts.append(i+1);
        }
    }
    return changedStarts;
}

void DiffEditor::goToNextChange()
{
    if (changedStarts.isEmpty())
    {
        qDebug() << "empty condition";
        return;
    }
    if (currentChangedIndex == -1)
    {
        qDebug() << "-1 condition";
        currentChangedIndex = 0 ;
        qDebug() << currentChangedIndex;
    }
    else
    {
        qDebug() << "else condition";

        currentChangedIndex = (currentChangedIndex + 1) % changedStarts.size();
        qDebug() << currentChangedIndex;
    }

    int blockNumber = changedStarts[currentChangedIndex];
    QTextBlock block = document()->findBlockByNumber(blockNumber);

    if (!block.isValid())
            return;

    verticalScrollBar()->setValue(blockNumber);
}

void DiffEditor::goToPreviousChange()
{
    if (changedStarts.isEmpty())
    {
        qDebug() << "empty condition";
        return;
    }

    int size = changedStarts.size();

    if (currentChangedIndex == -1)
    {
        qDebug() << "-1 condition";
        currentChangedIndex =  size - 1;
        qDebug() << currentChangedIndex;
    }
    else
    {
        qDebug() << "else condition";

        currentChangedIndex = (currentChangedIndex - 1 + size) % size;
        qDebug() << currentChangedIndex;
    }

    int blockNumber = changedStarts[currentChangedIndex];
    QTextBlock block = document()->findBlockByNumber(blockNumber);

    if (!block.isValid())
            return;

    verticalScrollBar()->setValue(blockNumber);
}

void DiffEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        goToNextChange();
        return;
    }
    else if (event->key() == Qt::Key_Left)
    {
        goToPreviousChange();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
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

void DiffEditor::updateLineNumberWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void DiffEditor::resizeEvent(QResizeEvent *event)
{
    QRect cr = contentsRect();

    QPlainTextEdit::resizeEvent(event);
    lineNumberArea->setGeometry(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height());
}

void DiffEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberWidth(0);
}

void DiffEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.setPen(QColor(Qt::black));

    QTextBlock block = this->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());

    int y = top + fontMetrics().ascent();

    int oldX = 5;
    int newX = lineNumberAreaWidth() / 2 + 5;

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (blockNumber > 0 && blockNumber - 1 < diffLines.size())
        {
            int diffIndex = blockNumber - 1;

            if (diffIndex >= 0 && diffIndex < diffLines.size())
            {
                const DiffLine &line = diffLines[diffIndex];
                QRect r(2, top, lineNumberAreaWidth(), blockBoundingGeometry(block).height());

                QColor addColor(200, 255, 200);
                QColor delColor(255, 200, 200);

                if (line.type == ADD)
                    painter.fillRect(r, addColor);

                if (line.type == DEL)
                    painter.fillRect(r, delColor);

                if (line.oldNum != -1)
                    painter.drawText(oldX, y, QString::number(line.oldNum));

                if (line.newNum != -1)
                    painter.drawText(newX, y, QString::number(line.newNum));
            }
        }
        top += blockBoundingGeometry(block).height();
        y = top + fontMetrics().ascent();

        block = block.next();
        ++blockNumber;
    }
}
