#include "nodelayer.h"

NodeLayer::NodeLayer(QGraphicsItem *parent):QGraphicsItem(parent)
{
}

void NodeLayer::addNode(float x, float y)
{
    Node* temp = new Node(this);
    temp->setPos(x,y);
    temp->setZValue(200);
    connect(temp,SIGNAL(pressed()),SIGNAL(pressed()));
    m_list.append(temp);
}

QList<Node*> NodeLayer::getList()
{
    return m_list;
}

void NodeLayer::clear()
{
    m_list.clear();
}
