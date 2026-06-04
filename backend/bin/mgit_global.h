#ifndef MGIT_GLOBAL_H
#define MGIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MGIT_LIBRARY)
#  define MGITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MGITSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MGIT_GLOBAL_H
