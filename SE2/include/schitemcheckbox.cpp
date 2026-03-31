#include "schitemcheckbox.h"


SchItemCheckBox::SchItemCheckBox(ItemType type,QString id,bool composite)
    : BaseItem(type,id)
{
    pw  = new QGraphicsProxyWidget(this);
    bkgWid = new QWidget();
    pw->setWidget(bkgWid);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    m_checkBox = new QCheckBox(bkgWid);
    m_checkBox->adjustSize();
    m_checkBox->show();
    bkgWid->adjustSize();
    bkgWid->resize(m_checkBox->size());
    pw->resize(bkgWid->size());

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);

        m_checkBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    connect(m_checkBox,SIGNAL(clicked(bool)),SLOT(updateValue(bool)));
    connect(this,SIGNAL(valueChanged()),SLOT(updateValue()));
    setZValue(2);
}

//SchItemCheckBox::~SchItemCheckBox()
//{
//    if(bkgWid)
//        bkgWid->deleteLater();
//}

void SchItemCheckBox::updateValue(bool value)
{
    setValue(QVariant(value));
}

void SchItemCheckBox::updateValue()
{
    qDebug()<<"checkBox set"<<getValue();
    if(m_checkBox)
        m_checkBox->setChecked(getValue().toBool());
}

QRectF SchItemCheckBox::boundingRect() const
{
    QRect BRect = bkgWid->rect();
    BRect.setSize(BRect.size().expandedTo(QSize(bkgWid->size().width()+4,bkgWid->size().height()+4)));
    BRect.moveTo(-2,-2);
    return BRect;
}

void SchItemCheckBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

ItemCheckBoxPlugin::~ItemCheckBoxPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

QGraphicsItem * ItemCheckBoxPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemCheckBox* item = new SchItemCheckBox(Checkbox,"",composite);
    m_list.append(item);
    return item;
}

void ItemCheckBoxPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemCheckBoxPlugin::type() const
{
    return Checkbox;
}

Q_EXPORT_PLUGIN2(ItemInterface, ItemCheckBoxPlugin)
