#ifndef SCHITEMLISTS_H
#define SCHITEMLISTS_H

#include <QObject>
#include "sch_interfaces.h"
#include "baseitem.h"
#include "QGraphicsProxyWidget"
#include "QMap"
#include "QGroupBox"
#include "QComboBox"
#include "QRadioButton"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QLineEdit"
#include "QSignalMapper"
#include "QPointF"

enum ItemListsType
{
    ItemComboBox,
    ItemGroupBox
};

struct ModeData
{
    ItemListsType type;
    bool horizontal;
};

class  SchItemLists : public BaseItem {
    Q_OBJECT

public:
    enum { Type = UserType + 4 };
    SchItemLists(ItemType type,QString id,bool composite=false);
    int type() const;


protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void changeType(ModeData data);

public slots:
    void changeMode();
    void clicked(int index);
    void updateLabels();
    void setElValue();

private:
    QWidget *wid;
    QGraphicsProxyWidget *pw;
    QMap<int,ModeData> mod_array;
    QGroupBox *groupBox;
    QComboBox *comboBox;
    QSignalMapper *signalMapper;
    bool m_composite;

};

class  ItemListsPlugin :public QObject, public ItemInterface
{
    Q_OBJECT
    Q_INTERFACES(ItemInterface)

    ~ItemListsPlugin ();

public:
    QGraphicsItem* createItem(bool composite, QGraphicsItem *parent);
    void removeItem(QGraphicsItem *item);
    ItemType type() const;

private:
    QList<QGraphicsItem*> m_list;
};

#endif // SCHITEMLISTS_H
