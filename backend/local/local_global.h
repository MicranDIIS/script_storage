#ifndef LOCAL_GLOBAL_H
#define LOCAL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LOCAL_LIBRARY)
#  define LOCALSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOCALSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LOCAL_GLOBAL_H
