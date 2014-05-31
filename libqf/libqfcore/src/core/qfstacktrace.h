#ifndef QFSTACKTRACE_H
#define QFSTACKTRACE_H

#include <qfcoreglobal.h>

//#include<QString>

//#include<string>
//namespace std {class string;};
class QString;

/// Helper class generating the current stack trace
/**
 If stacktrace_test seems not to work on MSVC platform,
 check if DbgHelp.dll is correct, ie NOT system one
 but one delivered with QFStackTrace
 (DbhHelp.dll have to be the same version as DbgHelp.lib used when libqf was compiled)
 Currently only the MSVC is supported.:((
*/
class QFCORE_DECL_EXPORT QFStackTrace
{
public:
	static QString stackTrace();
protected:
	static QString trace2str();
};

#endif // QFSTACKTRACE_H
