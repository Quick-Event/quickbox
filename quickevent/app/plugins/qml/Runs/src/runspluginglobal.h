#ifndef RUNSPLUGIN_GLOBAL_H
#define RUNSPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(RUNSPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define RUNSPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define RUNSPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
