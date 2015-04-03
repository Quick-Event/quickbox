#ifndef RECEIPESPLUGIN_GLOBAL_H
#define RECEIPESPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(RECEIPESPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define RECEIPESPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define RECEIPESPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
