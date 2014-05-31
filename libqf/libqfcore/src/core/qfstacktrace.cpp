#include "qfstacktrace.h"

#include <QStringList>

#include <sstream>

#if defined(Q_CC_MSVC)
	#include "msvc\SimpleSymbolEngine.h"
#elif defined Q_CC_GNU && !defined Q_CC_MINGW
	#include <execinfo.h>
#endif

#include <stdlib.h>

#if defined Q_CC_GNU && !defined Q_CC_MINGW && !defined QF_DISABLE_STACKTRACE
static QString demangle_gcc(const QString &_s)
{
	//qDebug() << __PRETTY_FUNCTION__ << _s;
	QString s = _s;
	QString ret;
	int first_digit_pos;
	for(first_digit_pos=0; first_digit_pos<_s.length(); first_digit_pos++) {
		QChar c = _s[first_digit_pos];
		if(c >= '0' && c <= '9') break;
	}
	if(first_digit_pos < _s.length()) {
		s = s.mid(first_digit_pos);
		int ix = 0;
		while(s[ix] >= '0' && s[ix] <= '9') ix++;
		QString s1 = s.mid(0, ix);
		s = s.mid(ix);
		ix = s1.toInt();
		ret += s.mid(0, ix);
		s = s.mid(ix);
		ix = 0;
		while(s[ix] >= '0' && s[ix] <= '9') ix++;
		s1 = s.mid(0, ix);
		s = s.mid(ix);
		ix = s1.toInt();
		QString s2 = s.mid(0, ix);
		if(!s2.isEmpty()) ret += "::" + s2;
		ret += "(" + s.mid(ix) + ")";
	}
	else ret = s;
	//qDebug() << "\t ret:" << ret;
	return ret;
}
#endif

QString QFStackTrace::stackTrace()
{
	QString ret;
#if defined Q_CC_GNU && !defined Q_CC_MINGW && !defined QF_DISABLE_STACKTRACE
	ret = trace2str();
	{
		QString s = ret;
		QStringList sl = s.split('\n'), sl_ret;
		int i = 0;
		foreach(s, sl) {
			int ix = s.indexOf('(');
			QString s1, s2, s3;
			if(ix > 0) {
				s1 = s.mid(0, ix);
				s = s.mid(ix+1);
				ix = s.indexOf('[');
				if(ix > 0) {
					s2 = s.mid(0, ix-2);
					s3 = s.mid(ix + 1);
					s3 = s3.mid(0, s3.length() - 1);
				}
				else
					s2 = s.mid(0, s.length() - 1);
			}
			else {
				ix = s.indexOf('[');
				if(ix > 0) {
					s1 = s.mid(0, ix-1);
					s3 = s.mid(ix + 1, s.length() - 1);
				}
				else s1 = s;
			}
			sl_ret << s1 + ";" + demangle_gcc(s2) + ";" + s3;
			i++;
		}
		ret = sl_ret.join("\n");
	}
#endif
	return ret;
}

#if defined(Q_CC_MSVC)
QString QFStackTrace::trace2str()
{
	std::ostringstream oss;

	CONTEXT context = {0};
	::GetThreadContext( GetCurrentThread(), &context );

	_asm call $+5
	_asm pop eax
	_asm mov context.Eip, eax
	_asm mov eax, esp
	_asm mov context.Esp, eax
	_asm mov context.Ebp, ebp

	SimpleSymbolEngine::instance().StackTrace( &context, oss );
	return oss.str();
}
#elif defined Q_CC_GNU && !defined Q_CC_MINGW && !defined QF_DISABLE_STACKTRACE
QString QFStackTrace::trace2str()
{
	static const int MAX_CNT = 100;
	void *array[MAX_CNT];
	int size;
	char **strings;
	std::ostringstream oss;

	size = backtrace (array, MAX_CNT);
	strings = backtrace_symbols (array, size);

	for(int i = 1; i < size; i++) {
		//qDebug() << strings[i];
		oss << strings[i] << "\n";
	}
	free(strings);
	return oss.str().c_str();
}
#elif !defined QF_DISABLE_STACKTRACE
///http://www.gnu.org/software/hello/manual/gnulib/execinfo_002eh.html
/**
8.13 execinfo.h

Declares the functions backtrace, backtrace_symbols, backtrace_symbols_fd.

Documentation:
http://www.gnu.org/software/libc/manual/html_node/Backtraces.html,
man backtrace.

Gnulib module: —

Portability problems fixed by Gnulib:

Portability problems not fixed by Gnulib:
This header file is missing on all non-glibc platforms:
	MacOS X 10.3, FreeBSD 6.0, NetBSD 3.0, OpenBSD 3.8, AIX 5.1, HP-UX 11, IRIX 6.5, OSF/1 5.1, Solaris 10,
	Cygwin, mingw, Interix 3.5, BeOS.
*/
QString QFStackTrace::trace2str() {return "NOT SUPPORTED";}
#endif
