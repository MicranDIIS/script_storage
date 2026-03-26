#include "../core/git_err.h"
#include <string.h>

char *errsys = NULL;

void core_errsys(void)
{
    const git_error *err = git_error_last();
    
    if(errsys != NULL){
        free(errsys);
        errsys = NULL;
    }

    if(err == NULL || err -> message == NULL){
        return;
    }

    size_t len = strlen(err -> message);
    char *msg = malloc(len + 1);
    if(msg != NULL){
        memcpy(msg,err -> message,len + 1);
        errsys = msg;
    }

}
