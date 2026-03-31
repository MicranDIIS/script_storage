#include "schitemgroupbox.h"


SchItemGroupBox::SchItemGroupBox(ItemType type,QString id,bool composite)
    : BaseItem(type,id)
{
    First_Node = new GroupBoxNode(this,composite);
    First_Node->setZValue(1);
    //qDebug()<<"Node1:"<<&First_Node;
    Second_Node = new GroupBoxNode(this,composite);
    Second_Node->setZValue(1);
    //qDebug()<<"Node2:"<<&Second_Node;
    Second_Node->setX(First_Node->x()+40);
    Second_Node->setY(First_Node->y()+40);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setValue(50);


    if(composite)
    {
        //First_Node->setFlag(ItemIsSelectable);
        First_Node->setFlag(ItemIsMovable);
        //Second_Node->setFlag(ItemIsSelectable);
        Second_Node->setFlag(ItemIsMovable);
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
    }

    setModeList(QStringList()<<"White"<<"Gray"<<"Red"<<"Green"<<"Blue"<<"Cyan"<<"Magenta"<<"Yellow");
    setZValue(-10);
}

void SchItemGroupBox::updateValue()
{
    if(getValue().toInt()<=0)
        setValue(1);
}

int ColorsId[]={3,6,7,8,9,10,11,12};
void SchItemGroupBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    QColor Color = (QColor)Qt::GlobalColor(ColorsId[getMode()]);
    painter->setPen(QPen(Color.lighter(getValue().toInt()*2*0.6), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Color.lighter(getValue().toInt()*2));
    painter->drawRect(boundingRect());
}


QRectF SchItemGroupBox::boundingRect() const
{

    QPointF Top_Left;
    QPointF Bot_Right;
    qreal x_s[]={First_Node->x(),Second_Node->x()};
    qreal y_s[]={First_Node->y(),Second_Node->y()};

    Top_Left.setX(*std::min_element(x_s,x_s+2)+3);
    Top_Left.setY(*std::max_element(y_s,y_s+2)+3);
    Bot_Right.setX(*std::max_element(x_s,x_s+2)+3);
    Bot_Right.setY(*std::min_element(y_s,y_s+2)+3);

    return QRectF(Top_Left,Bot_Right);
}

QPointF SchItemGroupBox::getP1()
{
    return First_Node->pos();
}

QPointF SchItemGroupBox::getP2()
{
    return Second_Node->pos();
}

void SchItemGroupBox::setP1(QPointF pos)
{
    First_Node->setPos(pos);
}

void SchItemGroupBox::setP2(QPointF pos)
{
    Second_Node->setPos(pos);
}







ItemGroupBoxPlugin::~ItemGroupBoxPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

ItemType ItemGroupBoxPlugin::type() const
{
    return SchGroupBox;
}

QGraphicsItem * ItemGroupBoxPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemGroupBox* item = new SchItemGroupBox(SchGroupBox,"",composite);
    m_list.append(item);
    return item;
}

void ItemGroupBoxPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

QPointF ItemGroupBoxPlugin::getP1(QGraphicsItem *item)
{
    SchItemGroupBox *obj = dynamic_cast<SchItemGroupBox*>(item);
    if(!obj) return QPointF();
    else return obj->getP1();
}



QPointF ItemGroupBoxPlugin::getP2(QGraphicsItem *item)
{
    SchItemGroupBox *obj = dynamic_cast<SchItemGroupBox*>(item);
    if(!obj) return QPointF();
    else return obj->getP2();
}


void ItemGroupBoxPlugin::setP1(QGraphicsItem *item, QPointF pos)
{  
    SchItemGroupBox *obj = dynamic_cast<SchItemGroupBox*>(item);
    if(obj) obj->setP1(pos);
}
void ItemGroupBoxPlugin::setP2(QGraphicsItem *item, QPointF pos)
{
    SchItemGroupBox *obj = dynamic_cast<SchItemGroupBox*>(item);
    if(obj) obj->setP2(pos);
}


Q_EXPORT_PLUGIN2(ItemGroupBoxInterface, ItemGroupBoxPlugin)
