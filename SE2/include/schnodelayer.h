#ifndef SCHNODELAYER_H
#define SCHNODELAYER_H
#include "sch_interfaces.h"
#include "node.h"

class SchNodeLayer:public QObject, public QGraphicsItem  {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    SchNodeLayer(QGraphicsItem* parent=0, bool composite=false);
    ~SchNodeLayer();
    void addNode(float x, float y);
    void clear();
    void updatePositions();
    QList<QGraphicsItem*> getNodes();
    void removeAllLines();

private:
    QList<Node*>  m_list;
    bool m_composite;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // SCHNODELAYER_H
