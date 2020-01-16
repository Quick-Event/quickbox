#ifndef SPEAKERPLUGIN_GLOBAL_H
#define SPEAKERPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(SPEAKERPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define SPEAKERPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define SPEAKERPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
