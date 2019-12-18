#ifndef RECEIPTSPLUGIN_GLOBAL_H
#define RECEIPTSPLUGIN_GLOBAL_H

#include <qglobal.h>

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(RECEIPTSPLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define RECEIPTSPLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define RECEIPTSPLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
