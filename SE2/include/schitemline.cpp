#include "schitemline.h"
const qreal Pi = 3.14;

SchItemLine::SchItemLine(bool composite,
                         QGraphicsItem *parent, QGraphicsScene *scene)
    : BaseItem(SchLine,"")
{
    m_StartItem = 0;
    m_EndItem = 0;
    setFlag(QGraphicsItem::ItemIsSelectable, composite);
    m_Color = Qt::black;
    m_itemOffset = 20;
    setZValue(-1);
    setModeList(QStringList()<<"No arrows"<<"Arrow to End"<<"Both arrows");
}

SchItemLine::~SchItemLine()
{
    qDebug("delete Line");
    if(m_StartItem)
        m_StartItem->removeLine(this);
    if(m_EndItem)
        m_EndItem->removeLine(this);
}

int SchItemLine::type() const
{
    return Type;
}

void SchItemLine::setColor(const QColor &color)
{
    m_Color = color;
}

Node * SchItemLine::startItem() const
{
    return m_StartItem;
}

Node * SchItemLine::endItem() const
{
    return m_EndItem;
}

void SchItemLine::updatePosition()
{
    updateMode();
}

QRectF SchItemLine::boundingRect() const
{
    qreal extra = (3 + 10) / 2.0;
    return QRectF(m_line1.p1(),m_line3.p2()).normalized().adjusted(-extra, -extra, extra, extra);;
}

void SchItemLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_StartItem || !m_EndItem)
        return;

    if(&m_StartItem==0|| &m_EndItem==0)
        return;

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
    painter->drawPolygon(m_arrowHead_Start);
    painter->drawPolygon(m_arrowHead_End);
}

void SchItemLine::updateMode()
{
    if(!m_StartItem || !m_EndItem)
        return;
    QGraphicsItem* startItemParent = m_StartItem->parentItem();
    QGraphicsItem* endItemParent = m_EndItem->parentItem();

    QPointF parentStartP = startItemParent->scenePos();
    QSizeF parentStartS = startItemParent->boundingRect().size();
    QPointF parentEndP = endItemParent->scenePos();
    QSizeF parentEndS = endItemParent->boundingRect().size();

    QPointF startP = m_StartItem->scenePos();
    QPointF endP = m_EndItem->scenePos();
    int h_offs = m_StartItem->boundingRect().height()/2;
    int w_offs = m_StartItem->boundingRect().width()/2;

    QLineF tline  = QLineF(startP,endP);
    if(abs(tline.dx())<abs(tline.dy()))
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
    //    QPolygonF polygon;
    //    polygon<<m_line1.p1()<<m_line2.p1()<<m_line3.p1()<<m_line3.p2();
    //    setPolygon(polygon);
    qreal arrowSize = 10;

    double angle_end = ::acos(m_line3.dx() / m_line3.length());
    if (m_line3.dy() >= 0)
        angle_end = (Pi * 2) - angle_end;

    QPointF arrowP1_end = m_line3.p2() - QPointF(sin(angle_end + Pi / 2.5) * arrowSize,
                                             cos(angle_end + Pi / 2.5) * arrowSize);
    QPointF arrowP2_end = m_line3.p2() - QPointF(sin(angle_end + Pi - Pi / 2.5) * arrowSize,
                                             cos(angle_end + Pi - Pi / 2.5) * arrowSize);

    double angle_start = ::acos(m_line1.dx() / m_line1.length());
    if (m_line1.dy() >= 0)
        angle_start = (Pi * 2) - angle_start;

    QPointF arrowP1_start = m_line1.p1() + QPointF(sin(angle_start + Pi / 2.5) * arrowSize,
                                             cos(angle_start + Pi / 2.5) * arrowSize);
    QPointF arrowP2_start = m_line1.p1() + QPointF(sin(angle_start + Pi - Pi / 2.5) * arrowSize,
                                             cos(angle_start + Pi - Pi / 2.5) * arrowSize);

    m_arrowHead_End.clear();
    m_arrowHead_Start.clear();
    switch(getMode()){
    case 2:
        m_arrowHead_Start << m_line1.p1() << arrowP1_start << arrowP2_start;
    case 1:
        m_arrowHead_End << m_line3.p2() << arrowP1_end << arrowP2_end;
        break;
    }
    update(boundingRect());
}

QPointF SchItemLine::getStartCoord()
{
    return m_line1.p1();
}

QPointF SchItemLine::getEndCoord()
{
    return m_line3.p2();
}

void SchItemLine::setBorderNodes(QGraphicsItem *start, QGraphicsItem *end)
{
    Node* n1 = qgraphicsitem_cast<Node*>(start);
    Node* n2 = qgraphicsitem_cast<Node*>(end);
    m_StartItem = n1;
    m_EndItem = n2;
    updateMode();
}

QPainterPath SchItemLine::shape()
{
    QPainterPath path;
    path.addPolygon(QPolygonF()<<m_line1.p1()<<m_line3.p2());
    path.addPolygon(m_arrowHead_Start);
    path.addPolygon(m_arrowHead_End);
    return path;
}

//QPainterPath SchItemLine::shape()
//{
//    QRectF rect = QRectF(m_line1.p1(),m_line3.p2()).normalized();
//    rect.adjust(-2,-2,2,2);
//    QPainterPath path;
//    path.addRect(rect);
////    qDebug()<<boundingRect();
//    return path;
//}
