#ifndef SCHITEMLINE_H
#define SCHITEMLINE_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include "sch_interfaces.h"
#include "node.h"
#include "baseitem.h"

class Node;

class SchItemLine: public BaseItem{

//    Q_OBJECT
public:

    enum { Type = UserType + 4 };
    enum Mode{ VerticalLink, HorizontalLink } ;
    SchItemLine(bool composite = false, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~SchItemLine();
    int type() const;
    void setColor(const QColor &color);
    void setBorderNodes(QGraphicsItem* start, QGraphicsItem* end);
    Node* startItem() const;
    Node* endItem() const;
    void updatePosition();
    QPointF getStartCoord();
    QPointF getEndCoord();

public slots:
    void updateMode();

protected:
    QRectF boundingRect() const;
    QPainterPath shape();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    Node *m_StartItem;
    Node *m_EndItem;
    QLineF m_line1;
    QLineF m_line2;
    QLineF m_line3;
    QPolygonF m_arrowHead_Start;
    QPolygonF m_arrowHead_End;
    QLineF m_start_line_offs;
    QLineF m_end_line_offs;
    QColor m_Color;
    Mode mode;
    int m_itemOffset;
};

#endif // SCHITEMLINE_H
