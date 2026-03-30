#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H
#include <QString>

class IScriptControl
{
public:
    virtual ~IScriptControl(){}
    virtual int Open () = 0;
    virtual int Close () = 0;
    virtual int GetError (QString* Str,QString* tb) = 0;
    virtual int LoadFile (char *Str) = 0;
    virtual int DoString (char *Str) = 0;
    virtual int DoFuncNoArg(char *Str) = 0;
    virtual int DoFunc1Arg(char *Str, char *arg1) = 0;
    virtual int Interrupt() = 0;
    virtual double GetGlobal (char *ValName ) = 0;
};

#endif // SCRIPTINTERFACE_H
