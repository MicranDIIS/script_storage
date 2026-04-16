#include <viewmodel.h>
#include <QDebug>

ViewModel::ViewModel (QObject *parent): QAbstractListModel(parent), viewMode(basicMode) {
}

//количество строк в отображении
int ViewModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()){ // у нас список, а не дерево, детей нет
        return 0;
    }
    return modeFiles.size();
}

//данные для индекса строки и роли для фильтрации
QVariant ViewModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if (row < 0 || row >= modeFiles.size()){
        return QVariant();
    }

    const FindFileInfo& info = modeFiles.at(row);

    if (role == Qt::DisplayRole){ //текст отображения
        return info.displayName;
    }

    if (role == RoleRole){ // для фильтра по ролям
        return info.roles;
    }

    if (role == DeviceRole){ // приборы и по аналогии дальше
        return info.devices;
    }

    if (role == StadeRole){
        return info.stades;
    }

    if (role == CategoryRole){
        return info.categories;
    }

    return QVariant();
}

//загружаем полный список файлов в модель
void ViewModel::setFiles(QList<FindFileInfo>& files){
    beginResetModel();
    allFiles = files;
    rebuildModeFiles();
    endResetModel();
//    qDebug() << "setFiles() input =" << files.size();
 }

//меняем режим
void ViewModel::setViewMode(ViewMode mode){
    if (viewMode == mode){
        return;
    }
    beginResetModel();
    viewMode = mode;
    rebuildModeFiles();
    endResetModel();
}

//ребилдим список отображаемых файлов в зависимости от режима
void ViewModel::rebuildModeFiles(){
    modeFiles.clear();
    for (int i = 0; i < allFiles.size(); i++ ){
        const FindFileInfo& currentFile = allFiles.at(i);

        if(viewMode == basicMode){
            if(currentFile.sourceFile == "remote"){
               modeFiles.append(currentFile);
            }
        }
        else {
            if (currentFile.sourceFile == "local" || currentFile.sourceFile == "preset"){
                modeFiles.append(currentFile);
            }
        }

    }
//     qDebug() << "rebuildModeFiles() result =" << modeFiles.size();
}


void ViewModel::clear(){
    beginResetModel();
    allFiles.clear();
    modeFiles.clear();
    endResetModel();
}

