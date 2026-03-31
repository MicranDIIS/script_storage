#ifndef SCHITEMFREENODE_H
#define SCHITEMFREENODE_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "node.h"

class  SchItemFreeNode: public BaseItem {
    Q_OBJECT
public:
    SchItemFreeNode(ItemType type,QString id,bool composite=false);
    ~SchItemFreeNode();

private:
    Node* newNode;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
};

class  SchItemFreeNodePlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~SchItemFreeNodePlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMFREENODE_H
