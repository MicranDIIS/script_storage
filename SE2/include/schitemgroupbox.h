#ifndef SCHITEMBUTTON_H
#define SCHITEMBUTTON_H

#include <QObject>
#include "sch_interfaces.h"
#include "groupboxnode.h"
#include "baseitem.h"
#include "QGraphicsProxyWidget"
#include "QPushButton"
#include <algorithm>
#include <QColor>
class SchItemGroupBox: public BaseItem {
    Q_OBJECT
public:
    SchItemGroupBox(ItemType type,QString id,bool composite=false);
    enum { Type = UserType + 8 };
    QPointF getP1();
    QPointF getP2();
    void setP1(QPointF pos);
    void setP2(QPointF pos);

public slots:
    void updateValue();
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    //SchNodeLayer* m_NodeLayer;
    GroupBoxNode* First_Node;
    GroupBoxNode* Second_Node;

};

class  ItemGroupBoxPlugin :public QObject, public ItemGroupBoxInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)
    Q_INTERFACES(ItemGroupBoxInterface)
    ~ItemGroupBoxPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    void setP1(QGraphicsItem *item, QPointF pos);
    void setP2(QGraphicsItem *item, QPointF pos);
    QPointF getP1(QGraphicsItem *item);
    QPointF getP2(QGraphicsItem *item);
    ItemType type() const;
    SchItemGroupBox * findObj(QGraphicsItem *item);
private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMBUTTON_H
