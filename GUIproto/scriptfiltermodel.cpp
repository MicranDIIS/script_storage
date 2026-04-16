#include "scriptfiltermodel.h"
#include <QDebug>

ScriptFilterModel::ScriptFilterModel(QObject *parent): QSortFilterProxyModel(parent), currentMode (ViewModel::basicMode) {

}

void ScriptFilterModel::resetAllFilters(){ // сбросить все фильтры (только для сброса фильтров при использовании поиска пока что используется)
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

// основной метод фильтрации строк
bool ScriptFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const{

    QAbstractItemModel *scriptsModel = sourceModel(); // загружаем модель
    if (!scriptsModel){
        qWarning() << "Model did not loaded correctly";
        return false;
    }

    QModelIndex index = scriptsModel->index(source_row,0,source_parent); // индекс строки исходной модели
    if(!index.isValid()){
        return false;
    }

    //получаем необходимые поля для фильтрации из FindFileInfo
    QStringList role = scriptsModel->data(index, ViewModel::RoleRole).toStringList();
    QStringList device = scriptsModel->data(index,ViewModel::DeviceRole).toStringList();
    QString text = scriptsModel->data(index,Qt::DisplayRole).toString();
    QString category = scriptsModel->data(index,ViewModel::CategoryRole).toString();
    QStringList stade = scriptsModel->data(index,ViewModel::StadeRole).toStringList();

    // логика отображения такая, что хотя бы один фильтр должен быть активен
    bool hasAnyFilter = false;
    if(currentMode == ViewModel::basicMode){
           hasAnyFilter = !roleFilter.isEmpty() || !deviceFilter.isEmpty() ||
                          !stadeFilter.isEmpty() || !categoryFilter.isEmpty() || !textSearch.isEmpty();
    } else {
        hasAnyFilter = !categoryFilter.isEmpty() || !textSearch.isEmpty();
    }

    if (!hasAnyFilter) {
        return false;
    }

    // фильтрация в зависимости от выбранного режима
    if (currentMode == ViewModel::basicMode){
        if (!roleFilter.isEmpty()){
             if(!role.contains(roleFilter)){
                return false;
            }
        }
        if (!deviceFilter.isEmpty()){
            if(!device.contains(deviceFilter)){
                return false;
            }
        }
        if (!stadeFilter.isEmpty()){
            if(!stade.contains(stadeFilter)){
                return false;
            }
         }
         if(!textSearch.isEmpty()){
            if(!text.contains(textSearch, Qt::CaseInsensitive))
            return false;
        }
        return true;
    }
    else {
        if(!categoryFilter.isEmpty()){
            if(category != categoryFilter)
                return false;
        }
        return true;
    }
    return true;
}
