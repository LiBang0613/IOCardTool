#ifndef IOCARD_GLOBAL_H
#define IOCARD_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IOCARD_LIBRARY)
#  define IOCARDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IOCARDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IOCARD_GLOBAL_H
