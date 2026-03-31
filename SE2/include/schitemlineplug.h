#ifndef SCHITEMLINEPLUG_H
#define SCHITEMLINEPLUG_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include "sch_interfaces.h"
#include "schitemline.h"

class  ItemLinePlugin :public QObject, public ItemLineInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)
    Q_INTERFACES(ItemLineInterface)

    ~ItemLinePlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;
    void setBorderNodes(QGraphicsItem *item, QGraphicsItem *startItem, QGraphicsItem *endItem);
    QPointF getP1(QGraphicsItem *item);
    QPointF getP2(QGraphicsItem *item);
    void updatePosition(QGraphicsItem *item);

private:
    QList<QGraphicsItem*> m_list;
    SchItemLine* findObj(QGraphicsItem* item);
};

#endif // SCHITEMLINEPLUG_H
