    #ifndef LUASANDBOX1_GLOBAL_H
#define LUASANDBOX1_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QVariant>
#include <QVector>

#if defined(LUASANDBOX1_LIBRARY)
#  define LUASANDBOX1SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LUASANDBOX1SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LUASANDBOX1_GLOBAL_H
