#ifndef SCRIPTIMPORTFUNC_H
#define SCRIPTIMPORTFUNC_H
#include <iostream>

enum eScriptType{
    LuaScript,
    PythonScript
};

typedef int (*fRegistrateFunc)(void* vm, std::string sModName, std::string *ClassName, eScriptType stype);
typedef std::string (*fGetErrFunc)();

#endif // SCRIPTIMPORTFUNC_H
