#include "diffeditor.h"

#include "linenumberarea.h"

DiffEditor::DiffEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setViewportMargins(40, 0, 0, 0);

    lineNumberArea = new LineNumberArea(this);

//    connect(this, &DiffEditor::blockCountChanged, this, &DiffEditor::updateLineNumberAreaWidth);
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
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

    lineNumberArea->update();
}
