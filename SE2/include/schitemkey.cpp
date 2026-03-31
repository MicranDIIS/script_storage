#include "schitemkey.h"

SchItemKey::SchItemKey(ItemType type,QString id, bool composite)
    :BaseItem(type,id)
{
//    mi_nodeLayer= 0;
    m_composite = composite;
    m_nodeLayer = 0;

    wid = new QWidget();
    QGraphicsProxyWidget *pw  = new QGraphicsProxyWidget(this);
    pw->setWidget(wid);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    wid->resize(51,45);
    pw->resize(51,45);

//    loadPlugins();

    ModeData mdata1;
    mdata1.node_list.append(QPointF(-5,4));
    mdata1.node_list.append(QPointF(-5,wid->height()-11));
    mdata1.node_list.append(QPointF(wid->width()-2,wid->height()/2-3));
    mdata1.icon.addFile(":/A1up.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata1.icon.addFile(":/A1down.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata1.bkg_style_string = "*{background-image: url(:/Key1.png);}";

    ModeData mdata1inv;
    mdata1inv.node_list.append(QPointF(-5,4));
    mdata1inv.node_list.append(QPointF(-5,wid->height()-11));
    mdata1inv.node_list.append(QPointF(wid->width()-2,wid->height()/2-3));
    mdata1inv.icon.addFile(":/A1Iup.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata1inv.icon.addFile(":/A1Idown.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata1inv.bkg_style_string = "*{background-image: url(:/Key1.png);}";

    ModeData mdata2;
    mdata2.node_list.append(QPointF(wid->width()-2,4));
    mdata2.node_list.append(QPointF(wid->width()-2,wid->height()-11));
    mdata2.node_list.append(QPointF(-5,wid->height()/2-3));
    mdata2.icon.addFile(":/A2up.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata2.icon.addFile(":/A2down.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata2.bkg_style_string = "*{background-image: url(:/Key2.png);}";

    ModeData mdata2inv;
    mdata2inv.node_list.append(QPointF(wid->width()-2,4));
    mdata2inv.node_list.append(QPointF(wid->width()-2,wid->height()-11));
    mdata2inv.node_list.append(QPointF(-5,wid->height()/2-3));
    mdata2inv.icon.addFile(":/A2Iup.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata2inv.icon.addFile(":/A2Idown.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata2inv.bkg_style_string = "*{background-image: url(:/Key2.png);}";

    ModeData mdata3;
    mdata3.node_list.append(QPointF(-5,wid->height()/2-3));
    mdata3.node_list.append(QPointF(wid->width()-2,wid->height()/2-3));
    mdata3.icon.addFile(":/A2up.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata3.icon.addFile(":/A3down.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata3.bkg_style_string = "*{background-image: url(:/Key3.png);}";

    ModeData mdata3inv;
    mdata3inv.node_list.append(QPointF(-5,wid->height()/2-3));
    mdata3inv.node_list.append(QPointF(wid->width()-2,wid->height()/2-3));
    mdata3inv.icon.addFile(":/A1up.png",QSize(51,45),QIcon::Normal,QIcon::On);;
    mdata3inv.icon.addFile(":/A3Idown.png",QSize(51,45),QIcon::Normal,QIcon::Off);
    mdata3inv.bkg_style_string = "*{background-image: url(:/Key3.png);}";

    mod_array.insert(0,mdata1);
    mod_array.insert(1,mdata1inv);
    mod_array.insert(2,mdata2);
    mod_array.insert(3,mdata2inv);
    mod_array.insert(4,mdata3);
    mod_array.insert(5,mdata3inv);
    setModeList(QStringList()<<"1"<<"1_inv"<<"2"<<"2_inv"<<"3"<<"3_inv");



    pb = new QPushButton(wid);

//    QIcon m_aIcon;
//    m_aIcon.addFile(":/Aup_r.png",QSize(59,59),QIcon::Normal,QIcon::On);
//    m_aIcon.addFile(":/Adwn_r.png",QSize(59,59),QIcon::Normal,QIcon::Off);

    pb->setStyleSheet("background: transparent");
    pb->setAutoFillBackground(false);
    pb->setCheckable(true);
    pb->resize(51,45);
//    pb->setIcon(m_aIcon);
    pb->setIconSize(QSize(51,45));
    pb->setFlat(true);
    pb->show();

    changeMode();

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
        pb->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    else
    {
        connect(pb,SIGNAL(clicked(bool)),SLOT(updateValue(bool)));
    }
    connect(this,SIGNAL(valueChanged()),SLOT(setElValue()));
    connect(this,SIGNAL(modeIndexChanged()),SLOT(changeMode()));
}

QGraphicsItem * ItemKeyPlugin::createItem(bool composite,QGraphicsItem *parent)
{
    SchItemKey* item = new SchItemKey(Key,"",composite);
    m_list.append(item);
    return item;
}

void ItemKeyPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemKeyPlugin::~ItemKeyPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}


QRectF SchItemKey::boundingRect() const
{
    QRect BRect = wid->rect();
    BRect.setSize(BRect.size().expandedTo(QSize(wid->size().width()+10,wid->size().height()+10)));
    BRect.moveTo(-5,-5);
    return BRect;
}

void SchItemKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

void SchItemKey::updateValue(bool val)
{
    setValue(QVariant(val));
}

ItemType ItemKeyPlugin::type() const
{
    return Key;
}

void SchItemKey::loadPlugins()
{
//    QDir dir(qApp->applicationDirPath());
//    if (!dir.cd("plugins")) {
//        return;
//    }
//    foreach (QString strFileName, dir.entryList(QDir::Files)) {
//        QPluginLoader loader(dir.absoluteFilePath(strFileName));
//        NodeLayerInterface* nli = qobject_cast<NodeLayerInterface*>(loader.instance());
//        if(nli)
//            mi_nodeLayer = nli;
//    }
}

int SchItemKey::type() const
{
    return Type;
}

void SchItemKey::setValue(QVariant value)
{
    qDebug()<<"key set value"<<value<<value.toBool();
    pb->setChecked(value.toBool());
    BaseItem::setValue(value);
}

void SchItemKey::setElValue()
{
    pb->setChecked(getValue().toBool());
}

void SchItemKey::changeMode()
{
    int mode = getMode();
    if(mode>mod_array.size()) return;
    const ModeData *data = &(mod_array.value(mode));
    wid->setStyleSheet(data->bkg_style_string);
    pb->setIcon(data->icon);
    qDebug()<<"change mode"<<mode<<data->icon<<data->bkg_style_string;

//    if(mi_nodeLayer)
//    {
        if(m_nodeLayer)
            delete m_nodeLayer;
//        qDebug()<<"change mode3";
        m_nodeLayer = new SchNodeLayer(this,m_composite);//mi_nodeLayer->createItem(m_composite,this);
        for(int i =0; i<data->node_list.size();i++)
        {
            m_nodeLayer->addNode(data->node_list.at(i).x(),data->node_list.at(i).y());
        }
//    }
}

Q_EXPORT_PLUGIN2(ItemInterface, ItemKeyPlugin)
