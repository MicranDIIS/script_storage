#include "scriptitemdelegate.h"

scriptItemDelegate::scriptItemDelegate(QObject *parent)
{
    Q_UNUSED(parent)
}

void scriptItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString baseText = index.model()->data(index, Qt::DisplayRole).toString();
    QIcon icon = qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole));
    QIcon icon2 = qvariant_cast<QIcon>(index.model()->data(index, Qt::UserRole));

    QPixmap iconPixmap = icon.pixmap(option.decorationSize);
    QPixmap iconPixmap2 = icon2.pixmap(option.decorationSize);

    QStyleOptionViewItem myoption =  option;
    QRect baseRect = option.rect;

    int space = ((icon.isNull() && icon2.isNull()) ? baseRect.height() : 0 ) + 3;
    QRect rectIcon1;
    rectIcon1.setX(baseRect.x() + space);
    rectIcon1.setY(baseRect.y());
    rectIcon1.setWidth(iconPixmap.size().width());
    rectIcon1.setHeight(baseRect.height());

    int space2 = iconPixmap2.width()==0 ? 0 : space;
    QRect rectIcon2;
    rectIcon2.setX(baseRect.x() + space2 + (icon2.isNull() ? 0 : rectIcon1.width()));
    rectIcon2.setY(baseRect.y());
    rectIcon2.setWidth(iconPixmap2.width());
    rectIcon2.setHeight(baseRect.height());

    QRect rectText;
    rectText.setX(baseRect.x() + rectIcon1.width() + rectIcon2.width() + space);
    rectText.setY(baseRect.y());
    rectText.setWidth(baseRect.width() - rectIcon1.width() - rectIcon2.width() - space);
    rectText.setHeight(baseRect.height());

    drawBackground(painter, myoption, index);
    drawDisplay(painter,myoption, rectText, baseText);
    drawDecoration(painter, myoption, rectIcon1, iconPixmap);
    drawDecoration(painter, myoption, rectIcon2, iconPixmap2);
    drawFocus(painter, myoption,option.rect);
}
