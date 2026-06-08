#include "combofiltermodel.h"

ComboFilterModel::ComboFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent), targetColumn(ViewModel::DisplayColumn) {
}

//запомнить столбец (поле фильтра)
void ComboFilterModel::setTargetColumn(ViewModel::Columns column) {
    targetColumn = column;
    rebuildUniqueValues();
}


//собираем уникальные значения для комбобоксов
void ComboFilterModel::rebuildUniqueValues() {
    beginResetModel();
    uniqueValues.clear();
    uniqueValues << QString::fromUtf8("Все");

    QAbstractItemModel *model = sourceModel();
    if (!model) {
        endResetModel();
        return;
    }
    for (int row = 0; row < model->rowCount(); ++row) {

        bool passFilters = true;
        QMap<ViewModel::Columns, QString>::const_iterator it;
        for (it = activeFilters.constBegin(); it != activeFilters.constEnd(); ++it) {

            QModelIndex filterIndex = model->index(row, it.key());

            if (!filterIndex.isValid()){
                passFilters = false;
                break;
            }

            QStringList filterParts = model->data(filterIndex, Qt::UserRole).toStringList();
            bool found = false;
            for (int j = 0; j < filterParts.size(); ++j) {
                if (filterParts.at(j) == it.value()) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                passFilters = false;
                break;
            }
        }
        if (!passFilters) {
            continue;
        }

        QModelIndex tIndex = model->index(row, targetColumn);
        if (!tIndex.isValid()){
         continue;
        }
        QStringList parts = model->data(tIndex,Qt::UserRole).toStringList();
        for (int i = 0; i < parts.size(); ++i) {
            QString value = parts.at(i);
            if (!value.isEmpty() && !uniqueValues.contains(value)) {
                uniqueValues << value;
            }
        }
    }
    uniqueValues.sort();
    uniqueValues.removeAll(QString::fromUtf8("Все"));
    uniqueValues.prepend(QString::fromUtf8("Все"));
    endResetModel();
}


int ComboFilterModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return uniqueValues.size();
}

QVariant ComboFilterModel::data(const QModelIndex& index, int role) const {

    if (!index.isValid() || index.row() >= uniqueValues.size()) {
        return QVariant();
    }

    QString value = uniqueValues.at(index.row());
    if (role == Qt::DisplayRole){
        return displayText(value);
    }
    if (role == Qt::UserRole) {
        return value;
    }

    return QVariant();
}

//зависимость фильтров
void ComboFilterModel::setFilter(ViewModel::Columns column, const QString& value) {
    if (value.isEmpty() || value == QString::fromUtf8("Все")) {
        activeFilters.remove(column);
    } else {
        activeFilters.insert(column, value);
    }
    rebuildUniqueValues();
}

//для русского отображения ролей, так как они валидируются ref_header
QString ComboFilterModel::displayText(const QString& value) const{
    if (targetColumn == ViewModel::RoleColumn) {
            if (value == "developer") {
                return QString::fromUtf8("Разработчик");
            }

            if (value == "production") {
                return QString::fromUtf8("Производство");
            }

            if (value == "metrolog") {
                return QString::fromUtf8("Метролог");

        }
    }
    return value;
}

void ComboFilterModel::clearFilters() {
    activeFilters.clear();
    rebuildUniqueValues();
}
