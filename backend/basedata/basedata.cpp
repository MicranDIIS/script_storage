#include "basedata.h"


Repo::Repo(const QString &path_) : repo(NULL), path(path_), token(""), error(""){
    if(path_.isEmpty()){
        error = "плохой путь";
    }

    git_repository *r = NULL;
    QByteArray Qpath_ = path_.toUtf8();
    if(git_repository_open(&r,Qpath_.constData()) != 0){
        error = "плохой путь";
    }else{
        repo = r;
    }
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

void Repo::set_token(const QString &token_){
    token = token_;
}

