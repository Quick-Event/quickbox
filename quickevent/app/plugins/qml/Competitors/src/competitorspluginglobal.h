#ifndef COMPETITTORSPLUGIN_GLOBAL_H
#define COMPETITTORSPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(COMPETITORSPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define COMPETITORSPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define COMPETITORSPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
