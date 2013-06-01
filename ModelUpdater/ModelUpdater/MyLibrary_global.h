#ifndef MYLIBRARY_GLOBAL_H
#define MYLIBRARY_GLOBAL_H

#include <QTCore/qglobal.h>

#if defined(MyLibrary_LIBRARY)
#define MyLibrary_EXPORT Q_DECL_EXPORT
#else
#define MyLibrary_EXPORT Q_DECL_IMPORT
#endif

#endif // MYLIBRARY_GLOBAL_H
