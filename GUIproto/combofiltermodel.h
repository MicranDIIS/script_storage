#ifndef COMBOFILTERMODEL_H
#define COMBOFILTERMODEL_H

#include <QSortFilterProxyModel>
#include "viewmodel.h"

class ComboFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit ComboFilterModel(QObject *parent = 0);

    void setTargetColumn(ViewModel::Columns column);
    void setFilter(ViewModel::Columns column, const QString &value);
    void clearFilters();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    void rebuildUniqueValues();
    QString displayText(const QString& value) const;

private:
    ViewModel::Columns targetColumn;
    QStringList uniqueValues;
    QMap<ViewModel::Columns, QString> activeFilters;
};

#endif // COMBOFILTERMODEL_H
