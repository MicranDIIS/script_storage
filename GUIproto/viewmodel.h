#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include "scriptloader.h"
#include <QAbstractListModel>
#include <QVariant>


class ViewModel: public QAbstractListModel
{
    Q_OBJECT

public:
    enum ViewMode {
        basicMode,
        customMode
    };

    enum CustomRoles { // пользовательские роли для комбобоксов
        RoleRole = Qt::UserRole + 1,
        DeviceRole,
        StadeRole,
        CategoryRole
    };

    explicit ViewModel(QObject *parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    void setFiles(QList<FindFileInfo>& files);
    void setViewMode(ViewMode mode);
    void clear();

private:

    QList<FindFileInfo> allFiles;
    QList<FindFileInfo> modeFiles;
    ViewMode viewMode;

    void rebuildModeFiles();
};

#endif // VIEWMODEL_H
