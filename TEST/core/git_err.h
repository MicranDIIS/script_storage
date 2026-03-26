#include <stdlib.h>
#include <git2.h>

extern char *errsys;
/*
    аналог errno
    Только хранит сразу текст
*/
void core_errsys(void);