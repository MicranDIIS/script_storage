#include "schitemfreenode.h"


SchItemFreeNode::SchItemFreeNode(ItemType type,QString id, bool composite)
    : BaseItem(type,id)
{
    newNode = new Node(this,composite);
    newNode->setZValue(100);

    if(composite)
    {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
}
SchItemFreeNode::~SchItemFreeNode()
{
    if(newNode)
        newNode->removeLines();
}

void SchItemFreeNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

QRectF SchItemFreeNode::boundingRect() const
{
    QRectF BRect = newNode->boundingRect();
    BRect.setSize(BRect.size().expandedTo(QSize(newNode->boundingRect().width()+10,newNode->boundingRect().height()+10)));
    BRect.moveTo(-3,-3);
    return BRect;
}

SchItemFreeNodePlugin::~SchItemFreeNodePlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

QGraphicsItem * SchItemFreeNodePlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemFreeNode* item = new SchItemFreeNode(FreeNode,"",composite);
    m_list.append(item);
    return item;
}

void SchItemFreeNodePlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType SchItemFreeNodePlugin::type() const
{
    return FreeNode;
}

Q_EXPORT_PLUGIN2(ItemInterface, SchItemFreeNodePlugin)
