#ifndef SCRIPTSMANAGER_H
#define SCRIPTSMANAGER_H

#include <QDir>
#include "include/sHead.h"
#include "include/findmodules.h"
#include "include/metadialog.h"

//#include <QStandardPaths>
#include <QPluginLoader>
#include "interfaces.h"
#include "models.h"

enum SortMode{
    BY_CATEGORY,
    BY_DIR,
    IN_ONE_LIST
};

typedef struct {
    int categoryIndex;
    int scriptIndex;
}stScriptIndexes;

struct stScriptFileInfo
{
    QString path;
    QString header;
    QString category;
};

struct stLoadFlags
{
    bool isEditNow;
    bool isEditStorage;
    bool isNetworkStorage;
    bool isConsole;
};

// поиск и хранение скриптов, модели отображения.
class ScriptsManager : public QObject, public ErrorManager
{
    Q_OBJECT
#ifdef TEST_MODE
    friend class tScriptsManager;
#endif

public:
    ScriptsManager(QWidget *parent);
    ~ScriptsManager();
    int FindProccedure();
    int SortFilesUpdateModel(SortMode mode);
    int FilterResults(QString filter);

    bool GetScriptHeader(const QString path, QString &specific, QString &category);
    stScriptIndexes GetScriptIndexes(QString path);
    int RowCount(const QModelIndex & index = QModelIndex());

    int UpdateScript(const QString &path);
    bool BackupScript(const QString &path);
    bool RestoreScript(const QString &path);

    bool IsUpdateAvailable(const QString& path);
    bool IsBackUpAvailable(const QString& path);

    bool CreateLinkToScript(const QString &path, const QString &header);
    bool CopyFromNetwork(const QString &sourecePath, const QString &suffix="", QString *tFileName=0);

    void SwitchToNetworkStorage(bool on);
    bool IsNetworkStorageExists();
    QWidget* GetPtrForm(){return mPiObj->GetPtrForm();}
    NameContentFilterModel* model()
    {
        return proxyModel;
    }

    int LoadScript(stScriptFileInfo info, stLoadFlags lFlags);
    int CreateNewScript(stScriptFileInfo info, int lang);
    QString GetAppPath(){return mp_finder->GetPath(APP_PATH);}

private:
    QStringList FindFiles(QString path, QStringList filter);
    QList<stScriptInfo> ExtractScriptsInfo(QStringList files);
    QList<stScriptInfo> CorrectTitles(QList<stScriptInfo> list);
    bool GetCategoryIndex(QList<stScriptInfo> list, QString category, int &index);
    QString getRelPath(QString fullPath);

    QPluginLoader *pl_obj;
    ModuleFinder* mp_finder;
    PluginInterface *mPiObj;
    TreeModel* treemodel;  // модель для отображения категорий и имён скрпитов
    NameContentFilterModel *proxyModel; // модель для фильтрации(поиску) по имени, содержимому файла

    QMap<QString,QString> mCategoryTitle;
    QMap<QString,QString> mTitlePath;
    QList<stScriptInfo> mScriptInfo;
    QStringList mFindedFiles;
    SortMode mSortMode;
    Updater *mpUpdater;
    BackUper *mpBackuper;
    QString m_backup_script_path;
    QString m_app_path;
    QString mScriptPath;
    QStringList m_scriptExclDir;
    bool m_netwStorage;

public slots:
    void clean();
};

#endif // SCRIPTSMANAGER_H
