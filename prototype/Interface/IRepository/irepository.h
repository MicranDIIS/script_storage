#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <FileStatus.h>
#include <CommitInfo.h>
#include <QString>
#include <QList>

class IRepository{
    public:
        virtual ~IRepository() {}

        //геттеры
        virtual const QString& get_path() const = 0;

        //логика с локальным репозиторием
        virtual QList<FileStatus> status() const = 0;
        virtual QList<CommitInfo> log() const = 0;
        virtual bool reset() = 0;

        //логика с удаленным репозиторием
        virtual bool clone() = 0;
        virtual bool pull() = 0;
        virtual bool fetch() = 0;

};

#endif
