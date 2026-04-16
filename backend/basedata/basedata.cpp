#include "basedata.h"


FileStatus::FileStatus(const QString &path_new_, const QString &path_old_, int flags_) : path_new(path_new_), path_old(path_old_), flags(flags_){}

const QString& FileStatus::get_path_new(void) const {
    return path_new;
}

const QString& FileStatus::get_path_old(void) const {
    return path_old;
}

bool FileStatus::flag_check(STATUS_FLAG flag) const {
    return flags & flag;
}


Repo::Repo(const QString &path_,const QString &token_) : repo(NULL), path(path_), token(token_), error(""){
    if(path_.isEmpty()){
        error = "плохой путь";
        return;
    }

    git_repository *r = NULL;
    QByteArray Qpath_ = path_.toUtf8();
    if(git_repository_open(&r,Qpath_.constData()) != 0){
        error = "Репозиторий не открылся";
    }else{
        repo = r;
    }
}

Repo Repo::open(const QString &path, const QString &token){
    return Repo(path,token);
}

Repo::~Repo(void){
    git_repository_free(repo);
}

const QString& Repo::get_path(void) const {
    return path;
}

const QString& Repo::get_error(void) const {
    return error;
}

const QString& Repo::get_token(void) const{
    return token;
}

bool Repo::valid(void)const {
    if(repo != NULL){
        return true;
    }

    return false;
}
