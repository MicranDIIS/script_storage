#ifndef SCRIPTCWRAPPER_H
#define SCRIPTCWRAPPER_H

#include "Libs_global.h"
#include "scriptinterface.h"
#include "../include/scriptimportfunc.h"

extern "C"{
LIB_DEST IScriptControl* CreateScriptWrapper(fRegistrateFunc pRegFunc, fGetErrFunc pGetErrFunc);
LIB_DEST void RemoveScriptWrapper(IScriptControl* sWrapper);
}

#endif // SCRIPTCWRAPPER_H
