#ifndef SCHITEMELEMENT_H
#define SCHITEMELEMENT_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "schnodelayer.h"
#include "QGraphicsProxyWidget"
#include "QMap"
#include "QPointF"

struct ModeData
{
    QString image_string;
    QList<QPointF> node_list;
};

class SchItemElement : public BaseItem {
    Q_OBJECT

public:
    enum { Type = UserType + 3 };
    SchItemElement(ItemType type,QString id,bool composite=false);
    int type() const;

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void changeMode();

private:
    void loadPlugins();
    bool m_composite;
//    NodeLayerInterface* mi_nodeLayer;
    SchNodeLayer *m_NodeLayer;
    QWidget *wid;
    QMap<int,ModeData> mod_array;
    QImage m_img;
};

class  ItemElemPlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~ItemElemPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};
#endif // SCHITEMELEMENT_H
