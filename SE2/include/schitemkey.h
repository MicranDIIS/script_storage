#ifndef SCHITEMKEY_H
#define SCHITEMKEY_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "schnodelayer.h"
#include "QGraphicsProxyWidget"
#include "QPushButton"
#include "QMap"

struct ModeData
{
    QString bkg_style_string;
    QIcon icon;
    QList<QPointF> node_list;
};

class  SchItemKey:  public BaseItem {
    Q_OBJECT

public:
    enum { Type = UserType + 18 };
    SchItemKey(ItemType type,QString id,bool composite=false);
    int type() const;
    void setValue(QVariant value);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void updateValue(bool);
    void setElValue();
    void changeMode();

private:
    void loadPlugins();
    bool m_composite;
//    NodeLayerInterface* mi_nodeLayer;
    SchNodeLayer *m_nodeLayer;
//    QGraphicsItem *m_NodeLayer;
    QWidget *wid;
    QPushButton*pb;
    QMap<int,ModeData> mod_array;
};

class  ItemKeyPlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~ItemKeyPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
//    void removeLines(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMKEY_H
