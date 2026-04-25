#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include "scriptloader.h"
#include <QStandardItemModel>
#include <QVariant>


class ViewModel: public QStandardItemModel
{
    Q_OBJECT

public:
    enum ViewMode {
        basicMode,
        customMode
    };

    enum Columns {
        DisplayColumn = 0,
        DeviceColumn,
        RoleColumn,
        StadeColumn,
        CategoryColumn,
        ColumnCount
    };

    enum CustomRoles{

    };

    explicit ViewModel(QObject *parent = 0);

    void setFiles(QList<FindFileInfo>& files);
    void setViewMode(ViewMode mode);

private:

    QList<FindFileInfo> allFiles;
    ViewMode viewMode;

    void rebuildModel();
};

#endif // VIEWMODEL_H
