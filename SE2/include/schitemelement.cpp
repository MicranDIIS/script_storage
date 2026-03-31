#include "schitemelement.h"


SchItemElement::SchItemElement(ItemType type,QString id, bool composite)
    :BaseItem(type,id)
{
//    mi_nodeLayer= 0;
    m_composite = composite;
    m_NodeLayer = 0;

    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

//    loadPlugins();

    ModeData mdataBlock;
    mdataBlock.node_list.append(QPointF(-5,19));
    mdataBlock.node_list.append(QPointF(49,19));
    mdataBlock.image_string = ":/Block.png";

    ModeData mdataChip5;
    for(int i=0;i<5;i++)
    {
        mdataChip5.node_list.append(QPointF(107,27+22*i));
    }
    mdataChip5.image_string = ":/Chip5.png";

    ModeData mdataChip7;
    for(int i=0;i<7;i++)
    {
        mdataChip7.node_list.append(QPointF(107,27+22*i));
    }
    mdataChip7.image_string = ":/Chip7.png";

    ModeData mdataCVG;
    mdataCVG.node_list.append(QPointF(-4,14));
    mdataCVG.node_list.append(QPointF(39,14));
    mdataCVG.image_string = ":/CVG.png";

    ModeData mdataDAC;
    mdataDAC.node_list.append(QPointF(97,14));
    mdataDAC.image_string = ":/DAC.png";

    ModeData mdataDiv;
    mdataDiv.node_list.append(QPointF(-5,19));
    mdataDiv.node_list.append(QPointF(49,19));
    mdataDiv.image_string = ":/Divider.png";

    ModeData mdataMixer;
    mdataMixer.node_list.append(QPointF(-4,14));
    mdataMixer.node_list.append(QPointF(39,14));
    mdataMixer.image_string = ":/Mixer.png";

    ModeData mdataMult;
    mdataMult.node_list.append(QPointF(-5,19));
    mdataMult.node_list.append(QPointF(49,19));
    mdataMult.image_string = ":/Multiplier.png";

    ModeData mdataOAmp;
    mdataOAmp.node_list.append(QPointF(-5,19));
    mdataOAmp.node_list.append(QPointF(49,19));
    mdataOAmp.image_string = ":/OpAmp.png";

    mod_array.insert(0,mdataBlock);
    mod_array.insert(1,mdataChip5);
    mod_array.insert(2,mdataChip7);
    mod_array.insert(3,mdataCVG);
    mod_array.insert(4,mdataDAC);
    mod_array.insert(5,mdataDiv);
    mod_array.insert(6,mdataMixer);
    mod_array.insert(7,mdataMult);
    mod_array.insert(8,mdataOAmp);


    setModeList(QStringList()<<"Block"<<"Chip5"<<"Chip7"<<"GCV"<<"DAC"<<"Divider"<<"Mixer"
                <<"Multiplier"<<"OpAmp");

    changeMode();

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
    }

    connect(this,SIGNAL(modeIndexChanged()),SLOT(changeMode()));
}

Q_EXPORT_PLUGIN2(ItemInterface, ItemElemPlugin)

int SchItemElement::type() const
{
    return Type;
}

QRectF SchItemElement::boundingRect() const
{
    QRect BRect = m_img.rect();
    BRect.setSize(BRect.size().expandedTo(QSize(m_img.size().width()+10,m_img.size().height()+10)));
    BRect.moveTo(-5,-5);
    return BRect;
}

void SchItemElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0,0,m_img);
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

void SchItemElement::loadPlugins()
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

QGraphicsItem * ItemElemPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemElement* item = new SchItemElement(SchElem,"",composite);
    m_list.append(item);
    return item;
}

void ItemElemPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemElemPlugin::~ItemElemPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

ItemType ItemElemPlugin::type() const
{
    return SchElem;
}

void SchItemElement::changeMode()
{
    int mode = getMode();
    if(mode>mod_array.size()) return;
    const ModeData *data = &(mod_array.value(mode));

//    if(mi_nodeLayer)
//    {
        if(m_NodeLayer)
            delete m_NodeLayer;
        m_NodeLayer = new SchNodeLayer(this,m_composite);//mi_nodeLayer->createItem(m_composite,this);
        for(int i =0; i<data->node_list.size();i++)
        {
            m_NodeLayer->addNode(data->node_list.at(i).x(),data->node_list.at(i).y());
        }
        m_img.load(data->image_string);
//    }
    update(boundingRect());
}
