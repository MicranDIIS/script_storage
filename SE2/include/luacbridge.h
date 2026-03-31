#ifndef LUASANDBOX1_H
#define LUASANDBOX1_H

#include "Libs_global.h"
#include "LuaRef.h"
#include <QMutex.h>
#include <QWaitCondition.h>

extern "C"{
    LIB_DEST int LuaOpen ();
    LIB_DEST int LuaClose ();
    LIB_DEST int LuaGetError (QString* Str,QString* tb);
    LIB_DEST int LuaDoString (char *Str);
    LIB_DEST int LuaLoadFile (char *Str);
    LIB_DEST int LuaDoFuncNoArg(char *Str);
    LIB_DEST double LuaGetGlobal (char *ValName );
    LIB_DEST QObject* LuaGetSynchObj();
    int errorHandler(lua_State* L);
    }

#endif // LUASANDBOX1_H
