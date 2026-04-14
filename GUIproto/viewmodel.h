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
    QString displayNameForItem(const FindFileInfo& info) const;
};

#endif // VIEWMODEL_H
