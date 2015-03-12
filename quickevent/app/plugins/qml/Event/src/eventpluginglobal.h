#ifndef EVENT_PLUGIN_GLOBAL_H
#define	EVENT_PLUGIN_GLOBAL_H

#include <qglobal.h>

#if defined(QE_EVENT_PLUGIN_BUILD_DLL)
//#warning "EXPORT"
#  define QE_EVENT_PLUGIN_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define QE_EVENT_PLUGIN_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif // EVENT_PLUGIN_GLOBAL_H
