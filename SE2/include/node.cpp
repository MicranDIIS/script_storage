#include "node.h"
#include "schitemline.h"

Node::Node(QGraphicsItem *parent, bool composite): QGraphicsItem(parent)
{
    m_composite = composite;
}


QRectF Node::boundingRect() const
{
    return QRectF(0,0,7,7);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_composite)
    {
        painter->setBrush(Qt::red);
        painter->drawRect(QRectF(0, 0, 6, 6));
    }
    Q_UNUSED(option);
    Q_UNUSED(widget);
}


void Node::addLine(QGraphicsItem *line)
{
    SchItemLine* newLine = qgraphicsitem_cast<SchItemLine*>(line);
    if(line)
        lines.append(newLine);
}

void Node::removeLine(QGraphicsItem *line)
{
    SchItemLine* newLine = qgraphicsitem_cast<SchItemLine*>(line);
    if(!newLine)
        return;

    lines.removeOne(newLine);
}

void Node::removeLines()
{
    //qDebug()<<"removeLines start";
    foreach (SchItemLine *line, lines) {
         //qDebug()<<"remove Line";
        line->scene()->removeItem(line); 
        delete line;
    }
    //qDebug()<<"removeLines end";
}

QVariant Node::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (SchItemLine *line, lines) {
            line->updatePosition();
        }
    }
    return value;
}

void Node::updateLinesPos()
{
    foreach (SchItemLine *line, lines) {
        line->updatePosition();
    }
}

