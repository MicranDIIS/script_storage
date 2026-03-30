#ifndef LUASANDBOX1_GLOBAL_H
#define LUASANDBOX1_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QVariant>
#include <QVector>

#if defined(LIBRARY)
#  define LIB_DEST Q_DECL_EXPORT
#else
#  define LIB_DEST Q_DECL_IMPORT
#endif

#endif // LUASANDBOX1_GLOBAL_H
