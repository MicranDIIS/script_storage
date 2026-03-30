#ifndef LIBSCOMMON_H
#define LIBSCOMMON_H

#if defined TEST
    #define DLL_EXPORT
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>

    #ifdef BUILD_DLL
        #define DLL_EXPORT __declspec(dllexport)
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
#else
    #define DLL_EXPORT
    #include "mtypedef.h"
#endif

#endif // LIBSCOMMON_H
