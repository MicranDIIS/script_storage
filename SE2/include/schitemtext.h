#ifndef SCHITEMTEXT_H
#define SCHITEMTEXT_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"

class SchItemTextObj: public QGraphicsTextItem {
    Q_OBJECT

public:
    enum { Type = UserType + 19 };
    SchItemTextObj();
    int type() const{return Type;}
    void setComposite(bool on);

public slots:
    void setText(QString txt);

protected:
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:
    void lostFocus(QGraphicsItem*);
    void itemSelected(QGraphicsItem*);
    void updateText(QVariant txt);

private:
    bool m_composed;
};

class SchItemText: public BaseItem {
    Q_OBJECT

public:
    SchItemText(ItemType type,QString id,bool composite=false);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void focusOutEvent(QFocusEvent *event);
    int type() const;
    void setFont(QFont font);
    int getMode();
    QFont getFont();

protected slots:
    void setText();

private:
    SchItemTextObj m_object;
    bool m_composite;
    bool m_editMode;
    QRectF boundingRect() const;

signals:
    void lostFocus(QGraphicsItem*);
    void itemSelected(QGraphicsItem*);
};

class  ItemTextPlugin :public QObject, public /*ItemInterface//*/ItemTextInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)
    Q_INTERFACES(ItemTextInterface)

    ~ItemTextPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;
    void setFont(QGraphicsItem *item, QFont font);
    QFont getFont(QGraphicsItem *item);
    int getMode(QGraphicsItem *item);

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMTEXT_H


