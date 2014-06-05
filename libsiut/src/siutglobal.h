#ifndef SIUT_GLOBAL_H
#define SIUT_GLOBAL_H

#include <qglobal.h>
/*
#if defined Q_CC_GNU && defined Q_OS_WIN32
# define QF_CC_MINGW
# define WINVER 0x0501 /// pro mingw jako, ze to je pro XP a vys
#endif
*/
/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(SIUT_BUILD_DLL)
//#warning "EXPORT"
#  define SIUT_DECL_EXPORT Q_DECL_EXPORT
#else
//#warning "IMPORT"
#  define SIUT_DECL_EXPORT Q_DECL_IMPORT
#endif

#endif 
