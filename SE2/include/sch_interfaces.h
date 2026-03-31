#ifndef SCHINTERFACES_H
#define SCHINTERFACES_H

#include <QtCore>
#include "QGraphicsItem"

enum ItemType {NoType=22, Key, Checkbox, Input, SchLine, SchText, SchElem, SchLists,FreeNode,SchButton,SchGroupBox};

class ItemInterface {
public:
    virtual ~ItemInterface() {}
    virtual QGraphicsItem* createItem(bool composite = false, QGraphicsItem* parent=0) = 0;
    virtual void removeItem(QGraphicsItem* ) = 0;
    virtual ItemType type() const = 0;
};


class ItemLineInterface : public ItemInterface {
public:
    virtual ~ItemLineInterface(){}
    virtual void setBorderNodes(QGraphicsItem *item, QGraphicsItem *startItem, QGraphicsItem *endItem)=0;
    virtual void updatePosition(QGraphicsItem *item) = 0;
    virtual QPointF getP1(QGraphicsItem *item) = 0;
    virtual QPointF getP2(QGraphicsItem *item) = 0;
};

class NodeLayerInterface : public ItemInterface {
public:
    virtual ~NodeLayerInterface() {}
    virtual  void addNode(QGraphicsItem* obj, float x, float y) = 0;
};

class ItemTextInterface  : public ItemInterface{
public:
    virtual ~ItemTextInterface() {}
    virtual void setFont(QGraphicsItem *item, QFont font) = 0;
    virtual QFont getFont(QGraphicsItem *item) = 0;
    virtual int getMode(QGraphicsItem *item) = 0;
};

class ItemInputInterface : public ItemInterface {
public:
    virtual ~ItemInputInterface() {}
    virtual void setMinValue(QGraphicsItem* obj,QVariant min) = 0;
    virtual void setMaxValue(QGraphicsItem* obj,QVariant min) = 0;
    virtual void setStepValue(QGraphicsItem* obj,QVariant step) = 0;
    virtual QVariant getMinValue(QGraphicsItem* obj) = 0;
    virtual QVariant getMaxValue(QGraphicsItem* obj) = 0;
    virtual QVariant getStepValue(QGraphicsItem* obj) = 0;
};

class ItemGroupBoxInterface : public ItemInterface {
public:
    virtual ~ItemGroupBoxInterface() {}
    virtual void setP1(QGraphicsItem* item, QPointF pos) = 0;
    virtual void setP2(QGraphicsItem* item, QPointF pos) = 0;
    virtual QPointF getP1(QGraphicsItem *item) = 0;
    virtual QPointF getP2(QGraphicsItem *item) = 0;
};

Q_DECLARE_INTERFACE(ItemInterface,"ru.YY.ItemInterface")
Q_DECLARE_INTERFACE(NodeLayerInterface,"ru.YY.NodeLayerInterface")
Q_DECLARE_INTERFACE(ItemLineInterface,"ru.YY.ItemLineInterface")
Q_DECLARE_INTERFACE(ItemTextInterface,"ru.YY.ItemTextInterface")
Q_DECLARE_INTERFACE(ItemInputInterface,"ru.YY.ItemInputInterface")
Q_DECLARE_INTERFACE(ItemGroupBoxInterface,"ru.YY.ItemGroupBoxInterface")
#endif // SCHINTERFACES_H
