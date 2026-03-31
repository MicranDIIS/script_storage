#include "line.h"
#include "math.h"

Line::Line(Node *startItem, Node *endItem,
          QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene)
{
//    qDebug("createLine");
    m_StartItem = startItem;
    m_EndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_Color = Qt::black;
    m_itemOffset = 20;

//    m_line1 = new QLineF(0,0,0,0);
//    m_line3 = new QLineF(0,0,0,0);
//    m_line2 = new QLineF(0,0,0,0);
    updateMode();
}

Line::~Line()
{
//    delete m_line1;
//    delete m_line2;
//    delete m_line3;
}

QRectF Line::boundingRect() const
{
//    qreal extra = (pen().width() + 20) / 2.0;

//    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
//                                      line().p2().y() - line().p1().y()))
//        .normalized()
//            .adjusted(-extra, -extra, extra, extra);
    return polygon().boundingRect();
}

//QPainterPath Line::shape() const
//{
//    QPainterPath path = QGraphicsLineItem::shape();
//////    path.addPolygon(arrowHead);
//    return path;
//}

void Line::updatePosition()
{
    updateMode();
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsItem* startItemParent = m_StartItem->parentItem();
    QGraphicsItem* endItemParent = m_EndItem->parentItem();
    if(startItemParent == endItemParent)
        return;

    painter->setPen(QPen(m_Color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(m_Color);
    if (isSelected()) {
        painter->setPen(QPen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(Qt::red);
    }

    updateMode();

    painter->drawLine(m_line1);
    painter->drawLine(m_line3);
    painter->drawLine(m_line2);
//    painter->drawPolygon(polygon());
}

void Line::updateMode()
{
    QGraphicsItem* startItemParent = m_StartItem->parentItem();
    QGraphicsItem* endItemParent = m_EndItem->parentItem();
    QPointF startP = m_StartItem->scenePos();
    QPointF endP = m_EndItem->scenePos();
    int h_offs = m_StartItem->boundingRect().height()/2;
    int w_offs = m_StartItem->boundingRect().width()/2;

    if(startP.x()>endP.x())
        mode = VerticalLink;
    else
        mode = HorizontalLink;

    int delta = (mode==VerticalLink) ? endP.y() - startP.y() : endP.x() - startP.x();

    m_line1.setP1(QPointF(startP.x()+w_offs,
                   startP.y()+h_offs));

    if(mode==VerticalLink)
    {
        m_line1.setP2(QPointF(startP.x()+w_offs,
                       startP.y()+h_offs+delta/2));
    }
    else
    {
        m_line1.setP2(QPointF(startP.x()+w_offs+delta/2,
                       startP.y()+h_offs));
    }

    m_line3.setP2(QPointF(endP.x()+w_offs,
                   endP.y()+h_offs));

    if(mode==VerticalLink)
    {
        m_line3.setP1(QPointF(endP.x()+w_offs,
                       startP.y()+h_offs+delta/2));
    }
    else
    {
        m_line3.setP1(QPointF(startP.x()+w_offs+delta/2,
                       endP.y()+h_offs));
    }

    m_line2.setP1(QPointF(m_line1.p2().x(),
                   m_line1.p2().y()));
    m_line2.setP2(QPointF(m_line3.p1().x(),
                    m_line3.p1().y()));

//    QLineF line(mapFromItem(m_StartItem, 0, 0), mapFromItem(m_EndItem, 0, 0));
    QPolygonF polygon;
    polygon<<m_line1.p1()<<m_line2.p1()<<m_line3.p1()<<m_line3.p2();
    setPolygon(polygon);
}
