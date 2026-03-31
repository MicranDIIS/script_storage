#ifndef GROUPBOXNODE_H
#define GROUPBOXNODE_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsScene>
#include<QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QLine>
#include <QRect>

class SchItemLine;

class GroupBoxNode : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 16 };

    GroupBoxNode(QGraphicsItem *parent = 0, bool composite = false);
    int type() const
        { return Type;}
    QRectF boundingRect() const;

signals:
    /* Сигнал, который будет посылаться в том случае,
     * если произошел клик мышью по объекту
     * */
    void pressed();

protected:
    /* Перегружаем метод нажатия мышью, для его перехвата
     * */
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    /* Данные методы виртуальные, поэтому их необходимо реализовать
     * в случае наследования от QGraphicsItem
     * */
    bool m_composite;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
//    bool eventFilter(QObject *, QEvent *);
};

#endif // GROUPBOXNODE_H
