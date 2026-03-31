#ifndef SCHITEMCHECKBOX_H
#define SCHITEMCHECKBOX_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "QGraphicsProxyWidget"
#include "QCheckBox"

class SchItemCheckBox : public BaseItem {
    Q_OBJECT

public:
    SchItemCheckBox(ItemType type,QString id,bool composite=false);

public slots:
    void updateValue(bool);
    void updateValue();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QCheckBox* m_checkBox;
    QWidget* bkgWid;
    QGraphicsProxyWidget *pw;
};

class  ItemCheckBoxPlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~ItemCheckBoxPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMCHECKBOX_H
