#ifndef UPDATE_HANDLER_H
#define UPDATE_HANDLER_H

#include <mgit.h>
#include <git2.h>
#include <QString>
#include <QObject>
#include <git_utils.h>

class UpdateHandler : public QObject{
    Q_OBJECT
private:
    git_repository* repo_;
    FileEventHandler Filehandler_;
    QString url_;
    QString token_;
    QString username_;
    QString branch_;
public:
    UpdateHandler(git_repository* repo, const FileEventHandler& Filehandler,
                  const QString& url, const QString& token,
                  const QString& username, const QString& branch);
    ~UpdateHandler(){}
signals:
    void updateIsFound();
private slots:
    void checkUpdatesActiveFile();
};

#endif
