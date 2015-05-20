#ifndef RECIPESPLUGIN_GLOBAL_H
#define RECIPESPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(RECIPESPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define RECIPESPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define RECIPESPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
