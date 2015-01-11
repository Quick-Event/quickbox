#ifndef QE_CORE_PLUGIN_GLOBAL_H
#define QE_CORE_PLUGIN_GLOBAL_H

#include <qglobal.h>

#if defined(QE_CORE_PLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define QE_CORE_PLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define QE_CORE_PLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif // QE_CORE_PLUGIN_GLOBAL_H
