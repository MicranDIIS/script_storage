#ifndef UPDATE_HANDLER_H
#define UPDATE_HANDLER_H

#include <mgit.h>
#include <git2.h>
#include <QString>
#include <QObject>
#include <git_raii.h>

class UpdateHandler : public QObject{
    Q_OBJECT
private:
    git_repository* repo_;
    FileEventHandler Filehandler_;
    QByteArray url_;
    QByteArray token_;
    QByteArray username_;
    QByteArray branch_;
public:
    UpdateHandler(git_repository* repo, const FileEventHandler& Filehandler,
                  const QByteArray& url, const QByteArray& token,
                  const QByteArray& username, const QByteArray& branch);
    ~UpdateHandler(){}
signals:
    void updateIsFound();
private slots:
    void checkUpdatesActiveFile();
};

#endif
