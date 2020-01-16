#ifndef CARDREADERPLUGIN_GLOBAL_H
#define CARDREADERPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(CARDREADERPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define CARDREADERPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define CARDREADERPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
