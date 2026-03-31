#include "schiteminput.h"


SchItemInput::SchItemInput(ItemType type,QString id,bool composite)
    : BaseItem(type,id)
{
     qDebug()<<"constructor Input";
    pw  = new QGraphicsProxyWidget(this);
    bkgWid = new QWidget();
    wid = new QWidget();
    pw->setWidget(bkgWid);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    m_isb = 0;
    m_dsb = 0;
    m_composite = composite;

    ModeData mdataInt;
    mdataInt.type = InputInt;
    mdataInt.enableStep = false;

    ModeData mdataIntStep;
    mdataIntStep.type = InputInt;
    mdataIntStep.enableStep = true;

    ModeData mdataDouble;
    mdataDouble.type = InputDouble;
    mdataDouble.enableStep = false;

    ModeData mdataDoubleStep;
    mdataDoubleStep.type = InputDouble;
    mdataDoubleStep.enableStep = true;

    mod_array.insert(0,mdataInt);
    mod_array.insert(1,mdataIntStep);
    mod_array.insert(2,mdataDouble);
    mod_array.insert(3,mdataDoubleStep);

    setModeList(QStringList()<<"Integer"<<"IntegerStep"<<"Double"<<"DoubleStep");

    m_btnStep = new QToolButton;
    connect(m_btnStep,SIGNAL(clicked()),SLOT(ChangeStep()));
    m_btnStep->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_btnStep->setArrowType(Qt::RightArrow);
    m_btnStep->adjustSize();
    m_btnStep->setMaximumWidth(15);
    m_btnStep->setMaximumHeight(17);

    layout = new QGridLayout;

    layout->addWidget(wid,0,0);
    layout->addWidget(m_btnStep,0,1);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setMargin(1);
    layout->setColumnStretch(1,0);
    layout->setColumnStretch(0,10);
    layout->setAlignment(m_btnStep,Qt::AlignLeft);
    bkgWid->setLayout(layout);

    pw->resize(bkgWid->size());
    changeMode();

    if(composite)
    {
        setFlag(ItemIsSelectable);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);

        m_btnStep->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

//    connect(m_isb,SIGNAL(valueChanged(int)),SLOT(updateValue(int)));
    connect(m_isb,SIGNAL(editingFinished()),SLOT(updateValue2()));
    connect(this,SIGNAL(valueChanged()),SLOT(updateValue()));
    connect(this,SIGNAL(modeIndexChanged()),SLOT(changeMode()));
    setZValue(2);
}

void SchItemInput::setType(InputType type, bool stepEnable)
{
    if(type==InputInt && !m_isb)
    {
        if(m_dsb)
        {
            m_dsb->deleteLater();
            m_dsb = 0;
        }
        m_isb = new QSpinBox(wid);
        m_isb->setMaximum(10000);
        m_isb->setMaximumHeight(17);
        m_isb->adjustSize();
        m_isb->setAlignment(Qt::AlignCenter);
        m_isb->show();
        if(m_composite)
            m_isb->setAttribute(Qt::WA_TransparentForMouseEvents);
        wid->resize(m_isb->size());
//        connect(m_isb,SIGNAL(valueChanged(int)),SLOT(updateValue(int)));
	connect(m_isb,SIGNAL(editingFinished()),SLOT(updateValue2()));
    }
    else if(type==InputDouble && !m_dsb){
        if(m_isb)
        {
            m_isb->deleteLater();
            m_isb = 0;
        }
        m_dsb = new QDoubleSpinBox(wid);
        m_dsb->setMaximum(10000);
        m_dsb->setMaximumHeight(17);
        m_dsb->adjustSize();
        m_dsb->setAlignment(Qt::AlignCenter);
        m_dsb->show();
        if(m_composite)
            m_dsb->setAttribute(Qt::WA_TransparentForMouseEvents);

        wid->resize(m_dsb->size());
//        connect(m_dsb,SIGNAL(valueChanged(double)),SLOT(updateValue(double)));
	connect(m_dsb,SIGNAL(editingFinished()),this,SLOT(updateValue2()));
    }

    layout->update();
    int bStepWidth = stepEnable ? m_btnStep->width() : 0;

    bkgWid->setMaximumHeight(17);
    bkgWid->resize((wid->width()+bStepWidth+2),wid->height());
    pw->resize(bkgWid->size());
    m_btnStep->setVisible(stepEnable);
    update(boundingRect());
}

void SchItemInput::updateValue(int val)
{
    setValue(QVariant(val));
}

void SchItemInput::updateValue(double val)
{
    setValue(QVariant(val));
}

void SchItemInput::updateValue()
{
    if(m_isb)
        m_isb->setValue(getValue().toInt());
    else if(m_dsb)
        m_dsb->setValue(getValue().toDouble());
}

QGraphicsItem * ItemInputPlugin::createItem(bool composite, QGraphicsItem *parent)
{
    SchItemInput* item = new SchItemInput(Input,"",composite);
    m_list.append(item);
    return item;
}

void ItemInputPlugin::removeItem(QGraphicsItem *item)
{
    if(item)
    {
        int index = m_list.indexOf(item);
        if(index>=0)
            m_list.removeAt(index);
        delete item;
    }
}

ItemType ItemInputPlugin::type() const
{
    return Input;
}

ItemInputPlugin::~ItemInputPlugin()
{
    foreach(QGraphicsItem* item,m_list)
        removeItem(item);
}

SchItemInput * ItemInputPlugin::findObj(QGraphicsItem *item)
{
    int index = m_list.indexOf(item);
    if(index<0)
        return 0;
    SchItemInput* obj = dynamic_cast<SchItemInput*>(item);
    return obj;
}

void SchItemInput::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        painter->setPen(Qt::red);
        painter->drawRect(boundingRect());
    }
}

QRectF SchItemInput::boundingRect() const
{
    QRect BRect = bkgWid->rect();
    BRect.setSize(BRect.size().expandedTo(QSize(bkgWid->size().width()+2,bkgWid->size().height()+2)));
    BRect.moveTo(-1,-1);
    return BRect;
}

Q_EXPORT_PLUGIN2(ItemInputInterface, ItemInputPlugin)

void SchItemInput::ChangeStep()
{
    if(m_isb)
        m_isb->setSingleStep( QInputDialog::getInt(0,tr("Шаг"),tr("Шаг, Гц"),m_isb->singleStep(),1,5000000));
    if(m_dsb)
        m_dsb->setSingleStep( QInputDialog::getDouble(0,tr("Шаг"),tr("Шаг, Гц"),m_dsb->singleStep(),1,5000000));
}

void SchItemInput::changeMode()
{
    int mode = getMode();
    if(mode>mod_array.size()) return;
    ModeData  data = (mod_array.value(mode));
    this->setType(data.type, data.enableStep);
}

void SchItemInput::setMaxValue(QVariant value)
{
    if(m_isb)
        m_isb->setMaximum(value.toInt());
    if(m_dsb) {
        m_dsb->setMaximum(value.toDouble());
    }
}

void SchItemInput::setMinValue(QVariant value)
{
    if(m_isb)
        m_isb->setMinimum(value.toInt());
    if(m_dsb) {
        m_dsb->setMinimum(value.toDouble());
    }
}

void SchItemInput::setStepValue(QVariant value)
{
    if(m_isb)
        m_isb->setSingleStep(value.toInt());
    if(m_dsb) {
        m_dsb->setSingleStep(value.toDouble());
    }
}

QVariant SchItemInput::getMaxValue()
{
    if(m_isb){
        return QVariant(m_isb->maximum());
    }
    if(m_dsb)
    {
        return QVariant(m_dsb->maximum());
    }
    return -1;
}

QVariant SchItemInput::getMinValue()
{
    if(m_isb)
        return QVariant(m_isb->minimum());
    if(m_dsb)
        return QVariant(m_dsb->minimum());
    return -1;
}

QVariant SchItemInput::getStepValue()
{
    if(m_isb)
        return QVariant(m_isb->singleStep());
    if(m_dsb)
        return QVariant(m_dsb->singleStep());
    return -1;
}

void ItemInputPlugin::setMinValue(QGraphicsItem *obj, QVariant min)
{
    SchItemInput* inp = findObj(obj);
    if(!inp) return;
    inp->setMinValue(min);
}

void ItemInputPlugin::setMaxValue(QGraphicsItem *obj, QVariant max)
{
    SchItemInput* inp = findObj(obj);
    if(!inp) return;
    inp->setMaxValue(max);
}

void ItemInputPlugin::setStepValue(QGraphicsItem *obj, QVariant step)
{
    SchItemInput* inp = findObj(obj);
    if(!inp) return;
    inp->setStepValue(step);
}

QVariant ItemInputPlugin::getMinValue(QGraphicsItem *obj)
{
    SchItemInput* inp = findObj(obj);
    //qDebug()<<"getMinValue"<<qgraphicsitem_cast<QGraphicsItem*>(inp);
    if(!inp) return -1;
    //qDebug()<<"return val"<<obj;
    return inp->getMinValue();

}

QVariant ItemInputPlugin::getMaxValue(QGraphicsItem *obj)
{
    SchItemInput* inp = findObj(obj);
    if(!inp) return -1;
    return inp->getMaxValue();
}

QVariant ItemInputPlugin::getStepValue(QGraphicsItem *obj)
{
    SchItemInput* inp = findObj(obj);
    if(!inp) return -1;
    return inp->getStepValue();
}

void SchItemInput::updateValue2()
{
    if(m_isb)
	setValue(m_isb->value());
    else if(m_dsb)
	setValue(m_dsb->value());
}

SchItemInput::~SchItemInput()
{
    if(wid)
	wid->deleteLater();
}
