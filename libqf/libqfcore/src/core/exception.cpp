#include "exception.h"
#include "stacktrace.h"
#include "log.h"

#include <QStringList>

#include <stdarg.h>
#include <stdio.h>

using namespace qf::core;

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
	#define logWarning() qWarning()
#else
	#define logWarning() qfWarning()
#endif

//============================================================
//                      Exception
//============================================================
bool Exception::s_abortOnException = false;

namespace {

bool isLogStackTrace()
{
	return true;
}

}

Exception::Exception(const QString &_msg, const QString &_where)
{
	m_where = _where;
	m_msg = _msg;
	m_what = m_msg.toUtf8();
	m_stackTrace = StackTrace::stackTrace().toString();
	log();
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

void Exception::setAbortOnException(bool on)
{
	s_abortOnException = on;
}

bool Exception::isAbortOnException()
{
	return s_abortOnException;
}

void Exception::fatalMessage(const QString &msg)
{
	qfError() << "EXCEPTION!:" << msg;
	abort();
}

