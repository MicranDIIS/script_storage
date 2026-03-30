#ifndef SCRIPTITEMDELEGATE_H
#define SCRIPTITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QItemDelegate>

class scriptItemDelegate : public QItemDelegate
{
public:
    scriptItemDelegate(QObject *parent = 0);

    void paint( QPainter * painter,
                const QStyleOptionViewItem & option,
                const QModelIndex & index ) const;
};

#endif // SCRIPTITEMDELEGATE_H
