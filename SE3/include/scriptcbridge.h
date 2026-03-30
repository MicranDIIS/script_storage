#ifndef LUASANDBOX1_H
#define LUASANDBOX1_H

#include "Libs_global.h"
#include "scriptinterface.h"

enum ScriptEngine
{
    LuaEngine,
    PythonEngine
};

extern "C"{
LIB_DEST IScriptControl* CreateScriptControl(ScriptEngine engine);
LIB_DEST void RemoveScriptControl(IScriptControl* sControl);
LIB_DEST QObject* GetSesManagerObj();
}

#endif // LUASANDBOX1_H
