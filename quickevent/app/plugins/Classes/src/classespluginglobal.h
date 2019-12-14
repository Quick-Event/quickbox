#ifndef CLASSESPLUGIN_GLOBAL_H
#define CLASSESPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(CLASSESPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define CLASSESPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define CLASSESPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
