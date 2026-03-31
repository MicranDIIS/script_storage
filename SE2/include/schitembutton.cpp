#include "schitembutton.h"


SchItemButton::SchItemButton(ItemType type,QString id,bool composite)
    : BaseItem(type,id)
{
    pw  = new QGraphicsProxyWidget(this);
    bkgWid = new QWidget();
    pw->setWidget(bkgWid);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    m_pushButton = new QPushButton(bkgWid);
    m_pushButton->adjustSize();
    m_pushButton->show();
    bkgWid->adjustSize();
    bkgWid->resize(m_pushButton->size());
    pw->resize(bkgWid->size());

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);

        m_pushButton->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    connect(m_pushButton,SIGNAL(clicked()),SLOT(updateValue()));
    connect(this,SIGNAL(valueChanged()),SLOT(updateLabel()));
    setZValue(2);
}

void SchItemButton::updateValue()
{
    setLabelList(QStringList());
}

void SchItemButton::updateLabel()
{
    m_pushButton->setText(getValue().toString());
    m_pushButton->setStyleSheet("padding-left: 4px;padding-right: 4px;padding-top: 10px;padding-bottom: 10px");
    m_pushButton->adjustSize();
    bkgWid->resize(m_pushButton->size());
    pw->resize(bkgWid->size());
}

QRectF SchItemButton::boundingRect() const
{
    QRect BRect = bkgWid->rect();
    BRect.setSize(BRect.size().expandedTo(QSize(bkgWid->size().width()+4,bkgWid->size().height()+4)));
    BRect.moveTo(-2,-2);
    return BRect;
}

void SchItemButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

ItemButtonPlugin::~ItemButtonPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

QGraphicsItem * ItemButtonPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemButton* item = new SchItemButton(SchButton,"",composite);
    m_list.append(item);
    return item;
}

void ItemButtonPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemButtonPlugin::type() const
{
    return SchButton;
}
Q_EXPORT_PLUGIN2(ItemInterface, ItemButtonPlugin)
