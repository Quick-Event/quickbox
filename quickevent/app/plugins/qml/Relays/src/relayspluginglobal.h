#pragma once

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(RELAYSPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define RELAYSPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define RELAYSPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

