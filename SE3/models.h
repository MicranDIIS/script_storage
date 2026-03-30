#ifndef MODELS_H
#define MODELS_H
#include "include/updater.h"
#include "include/backuper.h"
#include <QAbstractItemModel>
#include <QIcon>
#include <QMap>
#include <QSortFilterProxyModel>

typedef struct {
    QString category;
    QMap<QString,QString> script;
}stScriptInfo;

/// адаптация древовоидной модели под отображение в 2 списка

class TreeModel : public QAbstractItemModel
{
public:
    TreeModel(Updater* pUpdater, BackUper* pBackuper, QObject *parent = 0) : QAbstractItemModel(parent), mpUpdater(pUpdater), mpBackUper(pBackuper)
    {
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
        // сохраним указатель на родительский айтем для дальнейшего использования
        return createIndex(row, column, parent.isValid() ? list.at(parent.row()) : 0);
    }

    QModelIndex parent(const QModelIndex &child) const
    {
        // родительский индекс есть только у тех, для кого мы сохранили указатель в методе выше
        stScriptInfo *c = static_cast<stScriptInfo*>(child.internalPointer());
        return c ? createIndex(list.indexOf(c), 0) : QModelIndex();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return 1;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        bool hasParent = parent.isValid();
        // без этого условия получится бесконечная вложенность детей
        if (hasParent && parent.internalPointer())
            return 0;
        return hasParent ? list.at(parent.row())->script.size() : list.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        QVariant value;
        if (role != Qt::DisplayRole && role != Qt::ToolTipRole && role != Qt::DecorationRole && role != Qt::UserRole)
            return QVariant();

        QModelIndex parent = index.parent();
        if(parent.isValid())
        {
            switch (role) {
            case Qt::DisplayRole:
                value = list.at(parent.row())->script.keys().at(index.row());
                break;
            case Qt::ToolTipRole:
                value = list.at(parent.row())->script.values().at(index.row());
                break;
            case Qt::DecorationRole:
                value = mpUpdater->IsUpdateAvailable(list.at(parent.row())->script.values().at(index.row())) ? QIcon(":/upd") : QVariant();
                break;
            case Qt::UserRole:
                value = mpBackUper->CheckFileInArchive(list.at(parent.row())->script.values().at(index.row())) ? QIcon(":/restore_icon") : QVariant();
                break;
            default:
                value = list.at(parent.row())->category;
                break;
            }
        }
        else{

            if (role == Qt::DecorationRole){
                value = CheckCategoryUpdate(index.row()) ? QIcon(":/upd") : QVariant();
            }
            else
                value = list.at(index.row())->category;
        }
        return value;
    }

    void updateData(QList<stScriptInfo> &newData)
    {
        beginResetModel();
        list.clear();
        for(int i=0;i<newData.size();i++){
            stScriptInfo *elem = &newData[i];
            list.append(elem);
        }
//        qSort(list.begin(),list.end(),[](const stScriptInfo* a, const stScriptInfo* b ){return a->category.toLower()<b->category.toLower();});
        endResetModel();
    }

    bool CheckCategoryUpdate(int row) const
    {
        QStringList listVals = list.at(row)->script.values();
        for(int i=0; i<listVals.size(); i++){
            if(mpUpdater->IsUpdateAvailable(listVals.at(i)))
                return true;
        }
        return false;
    }

private:
    QList<stScriptInfo*> list; // данные модели
    Updater *mpUpdater; // для отображения наличия обновлений
    BackUper *mpBackUper; // для отображения наличия резервных копий
};


// модель для фильтрации по имени файла и/или по содержимому
// отображает только те файлы, которые содержат выражения из фильтров (по имени, по содержимому)

class NameContentFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
signals:
    void rowChanged();

public:
    NameContentFilterModel(QObject *parent) : QSortFilterProxyModel(parent){
        contentFilter="";
    }
    void setContentFilterText(QString filter){contentFilter=filter;invalidateFilter();}
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
    {

        QModelIndex currntIndex = sourceModel()->index(source_row, 0, source_parent);
        if(!currntIndex.isValid())
            return false;
         // check the current item
        bool result = sourceModel()->data(currntIndex).toString().contains(filterRegExp().pattern(),Qt::CaseInsensitive)
                &&  IsFilterContent(sourceModel()->data(currntIndex,Qt::ToolTipRole).toString());;

        if (sourceModel()->hasChildren(currntIndex)) {
        // if it has sub items
            for (int i = 0; i < sourceModel()->rowCount(currntIndex) && !result; ++i) {
                // keep the parent if a children is shown
                result = result || (filterAcceptsRow(i, currntIndex));
            }
        }
        return result;
    }

private:
    QString contentFilter;
    bool IsFilterContent(QString path) const
    {
        QFileInfo fi(path);
        if(!fi.isFile())
            return false;
        QFile file(path);
        if(file.open(QIODevice::ReadOnly))
        {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                line = in.readLine();
                if (line.contains(contentFilter,Qt::CaseInsensitive)) {
                    return true;
                }
            }
        }
        return false;
    }
};


#endif // MODELS_H
