#ifndef NODELAYER_H
#define NODELAYER_H
#include "node.h"
#include <QObject>

class NodeLayer : public QObject, public QGraphicsItem//public Node
{
    Q_OBJECT

public:
    NodeLayer(QGraphicsItem* parent=0);

    void addNode(float x,float y);
    void clear();
    QList<Node*> getList();
    int type() const
        { return Type;}

signals:
    void pressed();
    void released();

private:
    QList<Node*> m_list;
    QRectF boundingRect() const {return parentItem()->boundingRect();}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){}
};

#endif // NODELAYER_H
