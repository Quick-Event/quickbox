#ifndef COREPLUGIN_GLOBAL_H
#define COREPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(COREPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define COREPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define COREPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
