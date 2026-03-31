#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include<QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QLine>
#include <QRect>

class SchItemLine;

class Node : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 15 };

    Node(QGraphicsItem *parent = 0, bool composite = false);
     void addLine(QGraphicsItem *line);
     void removeLine(QGraphicsItem *line);
     void removeLines();
     void updateLinesPos();
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
    QList<SchItemLine *> lines;
//    bool eventFilter(QObject *, QEvent *);
};

#endif // NODE_H
