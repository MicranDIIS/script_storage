#ifndef BASEITEM_H
#define BASEITEM_H

#include <QObject>
#include <QVariant>
#include <QGraphicsItem>
#include "node.h"
#include "sch_interfaces.h"

class BaseItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    BaseItem(ItemType type,QString id,QString title = "");
    void setId(QString id);
    QVariant getValue();
    ItemType getType() const;
    void setType(ItemType type);
    QString getId() const;
    QString getTitle() const;
    QStringList getModeList() const;
    QStringList getLabelList() const;
    void setMode(int mode);
    int getMode();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setModeList(QStringList list);

public slots:
    void setValue(QVariant value);
    void setLabelList(QStringList list);

signals:
    void valueChanged();
    void modeIndexChanged();
    void labelListChanged();

private:
    QVariant m_vValue;
    ItemType m_type;
    QString m_id;
    QString m_title;
    QStringList m_labelList;
    QStringList m_modeList;
    int m_currMode;
};

#endif // BASEITEM_H
