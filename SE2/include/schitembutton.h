#ifndef SCHITEMBUTTON_H
#define SCHITEMBUTTON_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "QGraphicsProxyWidget"
#include "QPushButton"

class SchItemButton: public BaseItem {
    Q_OBJECT
public:
    SchItemButton(ItemType type,QString id,bool composite=false);

public slots:
    void updateValue();
    void updateLabel();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPushButton* m_pushButton;
    QWidget* bkgWid;
    QGraphicsProxyWidget *pw;
};

class  ItemButtonPlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~ItemButtonPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMBUTTON_H
