#ifndef EVENTPLUGIN_GLOBAL_H
#define EVENTPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(EVENTPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define EVENTPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define EVENTPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
