#ifndef ORISPLUGIN_GLOBAL_H
#define ORISPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(ORISPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define ORISPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define ORISPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
