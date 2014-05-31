#include <stdarg.h>
#include <stdio.h>

#include <QStringList>

#include "qfexception.h"
#include "qfstacktrace.h"

#include <qflogcust.h>

//============================================================
//                      QFExeption
//============================================================

bool QFException::f_exceptionAbortsApplication = false;
bool QFException::f_assertThrowsException = false;
bool QFException::f_logStackTrace = false;

bool QFException::isAssertThrowsException()
{
	return f_assertThrowsException;
}

void QFException::setAssertThrowsException(bool b)
{
	f_assertThrowsException = b;
}

bool QFException::isExceptionAbortsApplication()
{
	return f_exceptionAbortsApplication;
}

void QFException::setExceptionAbortsApplication(bool b)
{
	f_exceptionAbortsApplication = b;
}

bool QFException::isLogStackTrace()
{
	return f_logStackTrace;
}

void QFException::setLogStackTrace(bool b)
{
	f_logStackTrace = b;
}

void QFException::init(const QString& _type, const QString& _msg, const QString& _where)
{
	f_type = _type;
	fwhere = _where;
	fmsg = _type;
    	if(!fmsg.isEmpty()) fmsg += ": ";
	fmsg += _msg;
	fStackTrace = QFStackTrace::stackTrace();
	/*
	/// remove first 4 levels of stack (theese are exception initialization functions)
	QStringList sl = s.split("\n");
	for(int i=0; i<4 && sl.size()>1; i++) sl.removeAt(1); /// keep column captions
	fStackTrace = sl.join("\n");
	*/
	if(!fwhere.isEmpty()) recentExceptionRef() = *this;
	//log();
}

QFException& QFException::recentExceptionRef()
{
	static QFException e;
	return e;
}

void QFException::log(int level)
{
	if(isLogStackTrace())
		qfLog(level) << msg() << "\n" << where() << "\n----- stack trace -----\n" << stackTrace();
	else
		qfLog(level) << msg() << "\n" << where();
}

void QFException::log()
{
	log(QFLog::LOG_WARN);
}

const char* QFException::what() const throw()
{
	const_cast<QFException*>(this)->qstring2cstring_whatBuffer = fmsg.toLatin1();
	return qstring2cstring_whatBuffer.constData();
}
/*
const char* QFException::trace() const throw()
{
	QByteArray ba;
	ba = stackTrace().toLatin1();
	return ba.constData();
}
*/
void QFException::setGlobalFlags(int argc, char *argv[])
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

//============================================================
void QFInternalErrorException::init(const QString& _msg, const QString& _where)
{
	QFException::init("QFInternalErrorException", _msg, _where);
}

void QFInternalErrorException::log()
{
	qfLog(QFLog::LOG_ERR) << msg() << "\n" << where() << "\n----- stack trace -----\n" << stackTrace();
}

//============================================================
void QFSqlException::log()
{
	QFException::log(QFLog::LOG_INF);
}
