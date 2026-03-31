#include "baseitem.h"

BaseItem::BaseItem(ItemType type,QString id,QString title) : QGraphicsItem()
{
    m_vValue = 0;
    m_currMode = 0;
    m_type = type;
    m_id = id;
    m_title = title;
}

QVariant BaseItem::getValue()
{
    return m_vValue;
}

void BaseItem::setValue(QVariant value)
{
    if(value!=m_vValue)
    {
        m_vValue = value;
        emit valueChanged();
    }
}

ItemType BaseItem::getType() const
{
    return m_type;
}

QString BaseItem::getId() const
{
    return m_id;
}

QString BaseItem::getTitle() const
{
    return m_title;
}

void BaseItem::setId(QString id)
{
    m_id = id;
}

QRectF BaseItem::boundingRect() const
{
    return QRectF();
}

void BaseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void BaseItem::setType(ItemType type)
{
    m_type = type;
}

QStringList BaseItem::getModeList() const
{
    return m_modeList;
}

void BaseItem::setMode(int mode)
{
    if(mode!=m_currMode)
    {
        m_currMode = mode;
        emit modeIndexChanged();
    }
}

int BaseItem::getMode()
{
    return m_currMode;
}

void BaseItem::setModeList(QStringList list)
{
    m_modeList = list;
}

void BaseItem::setLabelList(QStringList list)
{
    m_labelList = list;
    labelListChanged();
}

QStringList BaseItem::getLabelList() const
{
    return m_labelList;
}
