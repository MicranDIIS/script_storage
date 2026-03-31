#ifndef LUASANDBOX1_H
#define LUASANDBOX1_H

#include "Libs_global.h"
#include <QMutex.h>
#include <QWaitCondition.h>

extern "C"{
    LIB_DEST int LuaOpen (QMutex *mutex,QWaitCondition *ready,bool *free);
    LIB_DEST int LuaClose ();
    LIB_DEST int LuaGetError (QString* Str);
    LIB_DEST int LuaDoString (char *Str);
    LIB_DEST double LuaGetGlobal (char *ValName );
    LIB_DEST int LuaServPlugin (QVector<QString> params,
                                                QVector<QVariant> args );
    LIB_DEST QObject* LuaGetSynchObj();
    }


#endif // LUASANDBOX1_H
