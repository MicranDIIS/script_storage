#ifndef SCRIPTBROKER_H
#define SCRIPTBROKER_H
#include "LuaRef.h"
#include "PythonQt.h"

extern void* scriptState;
extern QString scriptName;

void SetValInScript(QString name, QString value);

#endif // SCRIPTBROKER_H
