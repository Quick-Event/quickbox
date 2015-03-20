#include "exception.h"
#include "stacktrace.h"

#include <QStringList>
#include <QDebug>

#include <stdarg.h>
#include <stdio.h>

using namespace qf::core;

//============================================================
//                      Exception
//============================================================
bool Exception::terminateOnException = false;
/*
bool Exception::f_assertThrowsException = false;
bool Exception::f_logStackTrace = false;

bool Exception::isAssertThrowsException()
{
	return f_assertThrowsException;
}

void Exception::setAssertThrowsException(bool b)
{
	f_assertThrowsException = b;
}

bool Exception::isExceptionAbortsApplication()
{
	return f_exceptionAbortsApplication;
}

void Exception::setExceptionAbortsApplication(bool b)
{
	f_exceptionAbortsApplication = b;
}

bool Exception::isLogStackTrace()
{
	return f_logStackTrace;
}

void Exception::setLogStackTrace(bool b)
{
	f_logStackTrace = b;
}
*/
namespace {

bool isLogStackTrace()
{
	return true;
}

}

void Exception::init(const QString& _msg, const QString& _where)
{
	//m_type = _type;
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
/*
Exception& Exception::recentExceptionRef()
{
	static Exception e;
	return e;
}
*/
void Exception::log()
{
	if(isLogStackTrace())
		qWarning() << message() << "\n" << where() << "\n----- stack trace -----\n" << stackTrace();
	else
		qWarning() << message() << "\n" << where();
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
//============================================================
#if 0
void QFInternalErrorException::init(const QString& _msg, const QString& _where)
{
	Exception::init("QFInternalErrorException", _msg, _where);
}

void QFInternalErrorException::log()
{
	qfLog(QFLog::LOG_ERR) << message() << "\n" << where() << "\n----- stack trace -----\n" << stackTrace();
}
#endif
//============================================================
