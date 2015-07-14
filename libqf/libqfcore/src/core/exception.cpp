#include "exception.h"
#include "stacktrace.h"

#include <QStringList>
#include <QDebug>

#include <stdarg.h>
#include <stdio.h>

using namespace qf::core;

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
	#define logWarning() qWarning()
#else
	#define logWarning() qWarning().noquote()
#endif

//============================================================
//                      Exception
//============================================================
bool Exception::terminateOnException = false;

namespace {

bool isLogStackTrace()
{
	return true;
}

}

void Exception::init(const QString& _msg, const QString& _where)
{
	m_where = _where;
	m_msg = _msg;
	m_what = m_msg.toUtf8();
	m_stackTrace = StackTrace::stackTrace().toString();
	/*
	/// remove first 4 levels of stack (theese are exception initialization functions)
	QStringList sl = s.split("\n");
	for(int i=0; i<4 && sl.size()>1; i++) sl.removeAt(1); /// keep column captions
	m_stackTrace = sl.join("\n");
	*/
}

void Exception::log()
{
	if(isLogStackTrace())
		logWarning() << message() << "\n" << where() << "\n----- stack trace -----\n" << stackTrace();
	else
		logWarning() << message() << "\n" << where();
}

QString Exception::toString() const
{
	QString ret = "qf::core::Exception: ";
	ret += message();
	return ret;
}

const char* Exception::what() const throw()
{
	return m_what.constData();
}
/*
void Exception::setGlobalFlags(int argc, char *argv[])
{
	//setLogStackTrace(false);
	//setExceptionAbortsApplication(false);
	//setAssertThrowsException(false);
	for(int i=1; i<argc; i++) {
		QString s = argv[i];
		if(s == "--exception-aborts") setExceptionAbortsApplication(true);
		else if(s == "--assert-throws") setAssertThrowsException(true);
		else if(s == "--log-stacktrace") setLogStackTrace(true);
	}
}
*/
