#include "stacktrace.h"

#include "string.h"

#include <QStringList>

#include <sstream>
#include <stdlib.h>

#if defined Q_CC_GNU && !defined Q_CC_MINGW && !defined ANDROID
	#include <execinfo.h>
#endif

using namespace qf::core;

#if defined Q_CC_GNU && !defined Q_CC_MINGW

static QString cutName(QString &s)
{
	QString ret;
	int ix = 0;
	while(ix < s.length() && s[ix] >= '0' && s[ix] <= '9') {
		ix++;
	}
	if(ix < s.length()) {
		int l = s.mid(0, ix).toInt();
		ret = s.mid(ix, l);
		s = s.mid(ix + l);
	}
	return ret;
}

static QString demangle_gcc(const QString &_s)
{
	String s = _s;
	String ret;
	int first_digit_pos;
	for(first_digit_pos=0; first_digit_pos<_s.length(); first_digit_pos++) {
		QChar c = _s[first_digit_pos];
		if(c >= '0' && c <= '9')
			break;
	}
	if(first_digit_pos < s.length()) {
		s = s.slice(first_digit_pos);
		while(true) {
			QString ns = cutName(s);
			if(ns.isEmpty())
				break;
			if(ret.isEmpty())
				ret = ns;
			else
				ret += "::" + ns;
		}
		ret += "(" + s + ")";
	}
	else
		ret = s;
	//qDebug() << "\t ret:" << ret;
	return ret;
}

#endif

StackTrace::StackTrace(const QStringList &sl)
{
	m_trace = sl;
}

StackTrace StackTrace::stackTrace()
{
	QStringList sl_ret;
#if defined Q_CC_GNU && !defined Q_CC_MINGW
	QStringList sl = trace2str();
	int i = 0;
	for(String s : sl) {
		int ix = s.indexOf('(');
		String s1, s2, s3;
		if(ix > 0) {
			s1 = s.slice(0, ix);
			s = s.slice(ix+1);
			ix = s.indexOf('[');
			if(ix > 0) {
				s2 = s.slice(0, ix-2);
				s3 = s.slice(ix + 1, -1);
			}
			else s2 = s.slice(0, -1);
		}
		else {
			ix = s.indexOf('[');
			if(ix > 0) {
				s1 = s.slice(0, ix-1);
				s3 = s.slice(ix + 1, -1);
			}
			else s1 = s;
		}
		sl_ret << s1 + ";" + demangle_gcc(s2) + ";" + s3;
		i++;
	}
#endif
	return StackTrace(sl_ret);
}

QString StackTrace::toString() const
{
	return m_trace.join("\n");
}

QStringList StackTrace::trace() const
{
	return m_trace;
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
#elif defined Q_CC_GNU && !defined Q_CC_MINGW && !defined ANDROID
QStringList StackTrace::trace2str()
{
	static const int MAX_CNT = 100;
	void *array[MAX_CNT];
	int size;
	char **strings;
	QStringList ret;

	size = backtrace (array, MAX_CNT);
	strings = backtrace_symbols (array, size);

	for(int i = 1; i < size; i++) {
		//qDebug() << strings[i];
		ret << QString::fromUtf8(strings[i]);
	}
	free(strings);
	return ret;
}

#else
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
QStringList StackTrace::trace2str() {return QStringList();}
#endif
