#ifndef SCRIPTFILTERMODEL_H
#define SCRIPTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
#include "viewmodel.h"

class ScriptFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
     explicit ScriptFilterModel(QObject *parent = 0);

     void setMode(ViewModel::ViewMode mode);

     void setTextSearch(const QString& text);
     void setDeviceFilter(const QString& device);
     void setStadeFilter(const QString& stade);
     void setCategoryFilter(const QString& category);
     void setRoleFilter(const QString& role);
     void resetAllFilters();

protected:
     bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
private:
     ViewModel::ViewMode currentMode;

     QString textSearch;
     QString deviceFilter;
     QString stadeFilter;
     QString categoryFilter;
     QString roleFilter;
};

#endif // SCRIPTFILTERMODEL_H
