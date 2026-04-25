#include "scriptfiltermodel.h"
#include <QDebug>

ScriptFilterModel::ScriptFilterModel(QObject *parent): QSortFilterProxyModel(parent), currentMode (ViewModel::basicMode) {

}

void ScriptFilterModel::resetScriptFilters(){ // сбросить все фильтры
    roleFilter.clear();
    deviceFilter.clear();
    stadeFilter.clear();
    categoryFilter.clear();
    invalidateFilter();
}

void ScriptFilterModel::setMode(ViewModel::ViewMode mode){ // получаем режим, пересобираем фильтрацию
    if (currentMode == mode){
        return;
    }
    currentMode = mode;
    invalidateFilter();
}

// дальше методы для установки различных фильтров
void ScriptFilterModel::setDeviceFilter(const QString& device){
    if(deviceFilter == device){
        return;
    }
    deviceFilter = device;
    invalidateFilter();
}

void ScriptFilterModel::setTextSearch(const QString& text){
    if(textSearch == text){
        return;
    }
    textSearch = text;
    invalidateFilter();
}

void ScriptFilterModel::setCategoryFilter(const QString& category){
    if(categoryFilter == category){
        return;
    }
    categoryFilter = category;
    invalidateFilter();
}

void ScriptFilterModel::setStadeFilter(const QString& stade){
    if(stadeFilter == stade){
        return;
    }
    stadeFilter = stade;
    invalidateFilter();
}

void ScriptFilterModel::setRoleFilter(const QString& role){
    if(roleFilter == role){
        return;
    }
    roleFilter = role;
    invalidateFilter();
}
//проверка на включения нужного текста
bool ScriptFilterModel::containsText(int row, int column, const QModelIndex& parent, const QString& value) const{

    if (value.isEmpty()){
        return true;
    }
    if (value == QString::fromUtf8("Все")) {
        return true;
    }

    QModelIndex index = sourceModel()->index(row, column, parent);
    if (!index.isValid()){
        return false;
    }
    QStringList values = sourceModel()->data(index, Qt::UserRole).toStringList();

    for (int i = 0; i < values.size(); ++i) {
       if (values.at(i).trimmed() == value) {
           return true;
       }
   }
    return false;
}

// основной метод фильтрации строк
bool ScriptFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const{

    QAbstractItemModel *scriptsModel = sourceModel(); // загружаем модель
    if (!scriptsModel){
        qWarning() << "Model did not loaded correctly";
        return false;
    }

    QModelIndex index = scriptsModel->index(source_row, ViewModel::DisplayColumn ,source_parent);
    if(!index.isValid()){
        return false;
    }
    QString text = scriptsModel->data(index, Qt::DisplayRole).toString();

//     //логика отображения такая, что хотя бы один фильтр должен быть активен
//    bool hasAnyFilter = false;
//    if(currentMode == ViewModel::basicMode){
//           hasAnyFilter = !roleFilter.isEmpty() || !deviceFilter.isEmpty() || !stadeFilter.isEmpty() ;
//    } else {
//        hasAnyFilter = !categoryFilter.isEmpty() || !textSearch.isEmpty();
//    }

//    if (!hasAnyFilter) {
//        return false;
//    }

    // фильтрация в зависимости от выбранного режима
    if (currentMode == ViewModel::basicMode){
        if (!containsText(source_row, ViewModel::DeviceColumn, source_parent, deviceFilter)){
            return false;
        }
        if (!containsText(source_row, ViewModel::RoleColumn, source_parent, roleFilter)){
            return false;
        }

        if (!containsText(source_row, ViewModel::StadeColumn, source_parent, stadeFilter)){
            return false;
        }
    } else {
       if (!containsText(source_row, ViewModel::CategoryColumn, source_parent, categoryFilter)){
             return false;
      }
       if(!textSearch.isEmpty()){
           if(!text.contains(textSearch, Qt::CaseInsensitive)) {
                return false;
           }
         }
     }
  return true;
}




