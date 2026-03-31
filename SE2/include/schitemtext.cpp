#include "schitemtext.h"


SchItemText::SchItemText(ItemType type,QString id, bool composite)
    : BaseItem(type,id) /*, QGraphicsItem()*/
{
    if(composite){
        connect(&m_object,SIGNAL(itemSelected(QGraphicsItem*)),this,SIGNAL(itemSelected(QGraphicsItem*)));
        connect(&m_object,SIGNAL(lostFocus(QGraphicsItem*)),this,SIGNAL(lostFocus(QGraphicsItem*)));
    }
    connect(this,SIGNAL(valueChanged()),this,SLOT(setText()));
    connect(&m_object,SIGNAL(updateText(QVariant)),this,SLOT(setValue(QVariant)));
    if(composite)
    {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
    m_object.setComposite(composite);
    m_object.setParentItem(this);
    m_composite = composite;
    setValue("label");
    setZValue(2);
}

int SchItemText::type() const
{
    return SchText;//m_object.Type;
}

//QVariant SchItemTextObj::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
//{
//    if (change == QGraphicsItem::ItemSelectedHasChanged)
//        emit itemSelected(this);
//     if (change == QGraphicsItem::ItemPositionChange) {
//         qDebug()<<"change txtobj pos"<<pos()<<scenePos();
////         setPos(pos());
////         parentItem()->siz
//     }
//    return value;
//}

void SchItemTextObj::focusOutEvent(QFocusEvent *event)
{
    if(!m_composed) return;
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    updateText(QVariant(toPlainText()));
    QGraphicsTextItem::focusOutEvent(event);
}

void SchItemTextObj::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!m_composed) return;
    //    if (textInteractionFlags() == Qt::NoTextInteraction)
    //        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

QGraphicsItem * ItemTextPlugin::createItem(bool composite,QGraphicsItem *parent)
{
    SchItemText* item = new SchItemText(SchText,"",composite);
    m_list.append(item);
    return item;
}

void ItemTextPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemTextPlugin::type() const
{
    return SchText;
}

ItemTextPlugin::~ItemTextPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

SchItemTextObj::SchItemTextObj() :QGraphicsTextItem()
{
    //    setFlag(QGraphicsItem::ItemIsSelectable);
    m_composed = false;
}

void SchItemTextObj::setComposite(bool on)
{
    //    setFlag(QGraphicsItem::ItemIsSelectable,on);
    m_composed = on;
}

void SchItemText::setText()
{
    m_object.setText(getValue().toString());
}

void SchItemTextObj::setText(QString txt)
{
    setPlainText(txt);
}

QRectF SchItemText::boundingRect() const
{
    return  m_object.boundingRect();
}

void SchItemText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

void SchItemText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!m_composite) return;
    if (m_object.textInteractionFlags() == Qt::NoTextInteraction)
    {
        m_editMode = true;
        m_object.setFlag(ItemIsFocusable);
        m_object.setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void SchItemText::focusOutEvent(QFocusEvent *event)
{
    m_editMode = false;
    m_object.setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsItem::focusOutEvent(event);
}

void ItemTextPlugin::setFont(QGraphicsItem *item, QFont font)
{
    SchItemText *obj = dynamic_cast<SchItemText*>(item);
    if(!obj) return;
    obj->setFont(font);
}

void SchItemText::setFont(QFont font)
{
    m_object.setFont(font);
}

QFont SchItemText::getFont()
{
    return m_object.font();
}

QFont ItemTextPlugin::getFont(QGraphicsItem *item)
{
    SchItemText *obj = dynamic_cast<SchItemText*>(item);
    if(!obj) return QFont();
    return obj->getFont();
}



int SchItemText::getMode()
{
    return m_editMode;
}

int ItemTextPlugin::getMode(QGraphicsItem *item)
{
    SchItemText *obj = dynamic_cast<SchItemText*>(item);
    if(!obj) return 0;
    return obj->getMode();
}
Q_EXPORT_PLUGIN2(ItemTextInterface, ItemTextPlugin)
