#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>
#include <QDebug>
#include "node.h"

class Node;

class Line : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 4 };
   enum Mode{ VerticalLink, HorizontalLink } ;
    Line(Node *startItem, Node *endItem,
         QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~Line();
    int type() const
        { return Type; }
//    QRectF boundingRect() const;
//    QPainterPath shape() const;
    void setColor(const QColor &color)
        { m_Color = color; }
    Node *startItem() const
        { return m_StartItem; }
    Node *endItem() const
        { return m_EndItem; }

public slots:
    void updatePosition();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updateMode();


private:
    Node *m_StartItem;
    Node *m_EndItem;
    QLineF m_line1;
    QLineF m_line2;
    QLineF m_line3;
    QLineF m_start_line_offs;
    QLineF m_end_line_offs;
    QColor m_Color;
    Mode mode;
    int m_itemOffset;

};

#endif // LINE_H
