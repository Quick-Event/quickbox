#ifndef QUICKEVENT_GLOBAL_H
#define QUICKEVENT_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(QUICKEVENT_BUILD_DLL)
//#warning "EXPORT"
#  define QUICKEVENT_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define QUICKEVENT_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
