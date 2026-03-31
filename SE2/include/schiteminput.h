#ifndef SCHITEMINPUT_H
#define SCHITEMINPUT_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "QGraphicsProxyWidget"
#include "QGraphicsScene"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "QGridLayout"
#include "QToolButton"
#include "QInputDialog"

enum InputType
{
    InputInt,
    InputDouble
};

struct ModeData
{
    InputType type;
    bool enableStep;
};

class SchItemInput : public BaseItem {
    Q_OBJECT

public:
    enum { Type = UserType + 31 };
    SchItemInput(ItemType type,QString id,bool composite=false);
    ~SchItemInput();
    void setMaxValue(QVariant value);
    void setMinValue(QVariant value);
    void setStepValue(QVariant value);
    QVariant getMaxValue();
    QVariant getMinValue();
    QVariant getStepValue();

public slots:
    void updateValue(int val);
    void updateValue(double val);
    void updateValue2();
    void updateValue();
    void changeMode();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void ChangeStep();

private:
    bool m_composite;
    QWidget *wid;
    QWidget* bkgWid;
    QSpinBox* m_isb;
    QDoubleSpinBox* m_dsb;
    QPixmap pic;
    QToolButton *m_btnStep;
    QGridLayout *layout;
    QGraphicsProxyWidget *pw;
    QMap<int, ModeData> mod_array;

    void setType(InputType type, bool stepEnable);
};

class  ItemInputPlugin :public QObject, public ItemInputInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)
    Q_INTERFACES(ItemInputInterface)

    ~ItemInputPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    void setMinValue(QGraphicsItem *obj, QVariant min);
    void setMaxValue(QGraphicsItem *obj, QVariant max);
    void setStepValue(QGraphicsItem *obj, QVariant step);
    QVariant getMinValue(QGraphicsItem *obj);
    QVariant getMaxValue(QGraphicsItem *obj);
    QVariant getStepValue(QGraphicsItem *obj);
    ItemType type() const;

private:
    SchItemInput* findObj(QGraphicsItem* item);
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMINPUT_H
