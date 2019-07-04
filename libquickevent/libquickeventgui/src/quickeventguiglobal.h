#ifndef QUICKEVENTGUI_GLOBAL_H
#define QUICKEVENTGUI_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(QUICKEVENTGUI_BUILD_DLL)
//#warning "EXPORT"
#  define QUICKEVENTGUI_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define QUICKEVENTGUI_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
