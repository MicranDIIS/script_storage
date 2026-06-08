#include <viewmodel.h>
#include <QBrush>
#include <QColor>

ViewModel::ViewModel (QObject *parent): QStandardItemModel(parent), viewMode(basicMode) {
    setColumnCount(ColumnCount);
}


//загружаем полный список файлов в модель
void ViewModel::setFiles(QList<FindFileInfo>& files){

    allFiles = files;
    rebuildModel();

//    qDebug() << "setFiles() input =" << files.size();
 }

//меняем режим
void ViewModel::setViewMode(ViewMode mode){
    if (viewMode == mode){
        return;
    }
    viewMode = mode;
    rebuildModel();
}

//ребилд в зависимости от режима
void ViewModel::rebuildModel() {

    QStandardItemModel::clear();
    setColumnCount(ColumnCount);

    for (int i = 0; i < allFiles.size(); ++i) {
        const FindFileInfo& info = allFiles.at(i);

        if (viewMode == basicMode) {
            if (info.sourceFile != "remote")
                continue;
        } else {
            if (info.sourceFile != "local" && info.sourceFile != "preset")
                continue;
        }

        QList<QStandardItem*> row;

        QStandardItem *displayItem = new QStandardItem(info.displayName);
        displayItem->setData(info.absolutePath, ViewModel::FilePathRole);

        if (info.displayName.contains("[" + info.fileName + "]")) {
            displayItem->setForeground(QBrush(QColor(220, 20, 60)));// выделяем скрипты с одинаковыми метаданными цветом
        }

        QStandardItem *deviceItem = new QStandardItem();
        deviceItem->setData(info.devices, Qt::UserRole);

        QStandardItem *roleItem = new QStandardItem();
        roleItem->setData(info.roles, Qt::UserRole);

        QStandardItem *stadeItem = new QStandardItem();
        stadeItem->setData(info.stades, Qt::UserRole);

        QStandardItem *categoryItem = new QStandardItem(info.categories);
        categoryItem->setData(QStringList() << info.categories, Qt::UserRole);

        row.append(displayItem);
        row.append(deviceItem);
        row.append(roleItem);
        row.append(stadeItem);
        row.append(categoryItem);
        appendRow(row);

        for (int j = 0; j < row.size(); j++){
            row[j]->setEditable(false);
        }
    }
}




