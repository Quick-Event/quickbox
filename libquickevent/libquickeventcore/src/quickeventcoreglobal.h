#ifndef QUICKEVENTCORE_GLOBAL_H
#define QUICKEVENTCORE_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(QUICKEVENTCORE_BUILD_DLL)
//#warning "EXPORT"
#  define QUICKEVENTCORE_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define QUICKEVENTCORE_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
