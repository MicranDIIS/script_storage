#include "git_utils.h"

int callback(git_credential **out,const char *url,
                         const char *username_from_url,
                         unsigned int allowed_types,
                         void *payload)
{
    GitData* data = static_cast<GitData*>(payload);

    if(!data){
        return -1;
    }

    if(!(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)){
        return -1;
    }
    return git_credential_userpass_plaintext_new(out,
                                                 data -> username.constData(),
                                                 data -> token.constData());
}


int diff_file_callback(const git_diff_delta* delta, float progress, 
                              void* payload)
{
    (void)progress;
    LogFile* file = static_cast<LogFile*>(payload);
    
    const QString old_path = delta->old_file.path ? QString::fromUtf8(delta->old_file.path) : QString();
    const QString new_path = delta->new_file.path ? QString::fromUtf8(delta->new_file.path) : QString();
    
    if (old_path == file->file_path || new_path == file->file_path) {
        file -> found = true;
        return 1; 
    }
    
    return 0;
}


