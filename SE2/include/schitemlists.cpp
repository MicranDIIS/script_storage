#include "schitemlists.h"


SchItemLists::SchItemLists(ItemType type,QString id, bool composite)
    :BaseItem(type,id)
{
    pw  = new QGraphicsProxyWidget(this);
    wid = new QWidget();
    m_composite = composite;
    //     wid = 0;
    pw->setWidget(wid);
    groupBox = 0;
    comboBox = 0;
    //     wid = new QWidget();
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    ModeData mdataCombo;
    mdataCombo.type = ItemComboBox;
    mdataCombo.horizontal = false;

    ModeData mdataGroupH;
    mdataGroupH.type = ItemGroupBox;
    mdataGroupH.horizontal = true;

    ModeData mdataGroupV;
    mdataGroupV.type = ItemGroupBox;
    mdataGroupV.horizontal = false;

    mod_array.insert(0,mdataCombo);
    mod_array.insert(1,mdataGroupH);
    mod_array.insert(2,mdataGroupV);

    setModeList(QStringList()<<"ComboBox"<<"RadioGroupH"<<"RadioGroupV");

    updateLabels();

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);

    }
    connect(this,SIGNAL(labelListChanged()),SLOT(updateLabels()));
    connect(this,SIGNAL(modeIndexChanged()),SLOT(updateLabels()));
    connect(this,SIGNAL(valueChanged()),SLOT(setElValue()));
    setZValue(2);
//    BaseItem::setValue(0);
}

int SchItemLists::type() const
{
    return SchLists;
}

QRectF SchItemLists::boundingRect() const
{
    QRect BRect = pw->widget()->rect();
    BRect.setSize(BRect.size().expandedTo(QSize(pw->widget()->size().width()+2,pw->widget()->size().height()+2)));
    BRect.moveTo(-1,-1);
    return BRect;
}

void SchItemLists::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

void SchItemLists::changeType(ModeData data)
{
    if(comboBox)
    {
        comboBox->deleteLater();
        comboBox = 0;
    }
    if(groupBox)
    {
        groupBox->deleteLater();
        groupBox = 0;
    }

    if(data.type==ItemGroupBox/* && !groupBox*/)
    {
        groupBox = new QGroupBox(wid);
        groupBox->setFlat(true);
        if(m_composite)
            groupBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        wid->resize(groupBox->size());
        pw->resize(groupBox->size());
    }
    else if(data.type==ItemComboBox /*&& !comboBox*/)
    {
        comboBox = new QComboBox(wid);
        if(m_composite)
        {
            comboBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        }
        wid->resize(comboBox->size());
        pw->resize(comboBox->size());
    }
}

void SchItemLists::updateLabels()
{
    QStringList list = getLabelList();
    int mode = getMode();
    if(mode>mod_array.size()) return;
    const ModeData data = (mod_array.value(mode));

    changeType(data);
//    qDebug()<<"update labels"<<groupBox<<comboBox;
    if(groupBox)
    {
        if(groupBox->layout())
            delete groupBox->layout();
        qDeleteAll(groupBox->children());
        QHBoxLayout* layoutH = 0;
        QVBoxLayout* layoutV = 0;
        signalMapper = new QSignalMapper(groupBox);

        if(data.horizontal)
        {
            layoutH = new QHBoxLayout;
            layoutH->setMargin(0);
            layoutH->setSpacing(0);
            groupBox->setLayout(layoutH);
        }
        else
        {
            layoutV = new QVBoxLayout;
            layoutV->setMargin(0);
            layoutV->setSpacing(0);
            groupBox->setLayout(layoutV);
        }

        for(int i=0;i<list.size();i++)
        {
            QRadioButton* button = new QRadioButton(list.at(i));
            if(i==0)
                button->setChecked(true);
            if(m_composite)
                button->setAttribute(Qt::WA_TransparentForMouseEvents);
            connect(button,SIGNAL(clicked()),signalMapper,SLOT(map()));
            signalMapper->setMapping(button,i);
            groupBox->layout()->addWidget(button);
            groupBox->layout()->update();
        }

        connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(clicked(int)));

        groupBox->show();

        wid->resize(groupBox->size());
        pw->resize(groupBox->size());
    }
    else
    {
        comboBox->clear();
        comboBox->insertItems(0,list);
        comboBox->adjustSize();
        comboBox->show();

        wid->resize(comboBox->size());
        pw->resize(comboBox->size());

        if(m_composite)
            comboBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(clicked(int)));

//        for (int i = 0 ; i < comboBox->count() ; ++i) {
//        comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
//        }
    }
//    qDebug()<<"updateLabels2"<<wid->size()<<pw->size();
    update(boundingRect());
}

void SchItemLists::changeMode()
{
    int mode = getMode();
    if(mode>mod_array.size()) return;
    const ModeData data = (mod_array.value(mode));
    changeType(data);
}

void SchItemLists::clicked(int index)
{
    setValue(QVariant(index));
}

Q_EXPORT_PLUGIN2(ItemInterface, ItemListsPlugin)

QGraphicsItem * ItemListsPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemLists* item = new SchItemLists(SchLists,"",composite);
    m_list.append(item);
    return item;
}

void ItemListsPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemListsPlugin::type() const
{
    return SchLists;
}

ItemListsPlugin::~ItemListsPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

void SchItemLists::setElValue()
{
    QVariant value = getValue();
    int index = value.toInt();
    if(groupBox)
    {
        qDebug()<<groupBox->children();
        QRadioButton * rb = dynamic_cast<QRadioButton*>(groupBox->children().at(index+2));
        if(rb)
            rb->setChecked(true);
    }
    else
        comboBox->setCurrentIndex(index);
}
