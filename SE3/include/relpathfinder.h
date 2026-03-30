#ifndef RELPATHFINDER_H
#define RELPATHFINDER_H

#include <QObject>
#include <QThread>

class RelPathFinder : public QThread
{
    Q_OBJECT
public:
    RelPathFinder(const QString& baseFileName, const QStringList& fileList, const QString& rootShift="./", int deepScanV = 1, int deepScanH = 4);

    void run() override;
    QString FullToRelPath(const QString& fullPath);
    bool FindAppDir(const QString& path, QString &resPath, int currDeep);

    QString m_appfile;
    QStringList m_fileList;
    QString m_relroot;
    int m_vDeep;
    int m_hDeep;

signals:
    void resultReady(QStringList s);
};

#endif // RELPATHFINDER_H
