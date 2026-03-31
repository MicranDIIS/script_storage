#include "groupboxnode.h"
#include "schitemline.h"

GroupBoxNode::GroupBoxNode(QGraphicsItem *parent, bool composite): QGraphicsItem(parent)
{
    m_composite = composite;
}


QRectF GroupBoxNode::boundingRect() const
{
    return QRectF(0,0,7,7);
}

void GroupBoxNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_composite)
    {
        painter->setBrush(Qt::cyan);
        painter->drawRect(QRectF(0, 0, 6, 6));
    }
    Q_UNUSED(option);
    Q_UNUSED(widget);
}


QVariant GroupBoxNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return value;
}

