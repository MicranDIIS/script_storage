#include "schitemlineplug.h"

QGraphicsItem * ItemLinePlugin::createItem(bool composite,QGraphicsItem *parent)
{
    SchItemLine *obj = new SchItemLine(composite,parent);
    m_list.append(obj);
    return obj;
}

void ItemLinePlugin::removeItem(QGraphicsItem *item)
{
    if(int index = m_list.indexOf(item)>-1)
    {
        m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemLinePlugin::type() const
{
    return SchLine;
}

ItemLinePlugin::~ItemLinePlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

void ItemLinePlugin::updatePosition(QGraphicsItem *item)
{
    SchItemLine *obj = qgraphicsitem_cast<SchItemLine*>(item);
    if(obj)
        obj->updatePosition();
}

void ItemLinePlugin::setBorderNodes(QGraphicsItem *item, QGraphicsItem *startItem, QGraphicsItem *endItem)
{
    int index = m_list.indexOf(item);
    if(index<0)
        return;
    SchItemLine* obj = qgraphicsitem_cast<SchItemLine*>(item);
    if(!obj) return;
    obj->setBorderNodes(startItem,endItem);
}



QPointF ItemLinePlugin::getP1(QGraphicsItem *item)
{
    SchItemLine*tmp = findObj(item);
    if(!tmp)
        return QPointF();
    else
        return tmp->getStartCoord();
}



QPointF ItemLinePlugin::getP2(QGraphicsItem *item)
{
    SchItemLine*tmp = findObj(item);
    if(!tmp)
        return QPointF();
    else
        return tmp->getEndCoord();
}
Q_EXPORT_PLUGIN2(ItemLineInterface, ItemLinePlugin)

SchItemLine* ItemLinePlugin::findObj(QGraphicsItem *item)
{
    int index = m_list.indexOf(item);
    if(index<0)
        return 0;
    SchItemLine* obj = qgraphicsitem_cast<SchItemLine*>(item);
    return obj;
}
