#include "schnodelayer.h"


SchNodeLayer::SchNodeLayer(QGraphicsItem* parent,bool composite)
    : QGraphicsItem(parent)
{
    m_composite = composite;
}

SchNodeLayer::~SchNodeLayer()
{
    removeAllLines();
}

void SchNodeLayer::addNode(float x, float y)
{
    Node* newNode = new Node(this,m_composite);
    newNode->setPos(x,y);
    newNode->setZValue(100);
    m_list.append(newNode);
}

void SchNodeLayer::clear()
{
    m_list.clear();
}

void SchNodeLayer::updatePositions()
{
    foreach(Node* node, m_list)
    {
        node->updateLinesPos();
    }
}

QRectF SchNodeLayer::boundingRect() const
{
    return parentItem()->boundingRect();
}

void SchNodeLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{   Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QList<QGraphicsItem *> SchNodeLayer::getNodes()
{
    QList<QGraphicsItem*> list;
    foreach(Node* node,m_list)
    {
        list.append(node);
    }
    return list;
}

void SchNodeLayer::removeAllLines()
{
    foreach(Node*node,m_list)
    {
        node->removeLines();
    }
}

