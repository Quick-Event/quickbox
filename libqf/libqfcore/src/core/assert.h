#ifndef QFASSERT_H
#define QFASSERT_H

#include <qfexception.h>

#define QF_ASSERT(cond, msg) \
	if(!(cond)) { \
		QFLog(QFLog::LOG_ERR, "StackTrace:") << QFLog::stackTrace(); \
		if(QFException::isAssertThrowsException()) throw QFInternalErrorException("ASSERT(" + QString(#cond) + ") " + msg, QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME)); \
		else QFLog(QFLog::LOG_FATAL, "ASSERT") << QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME) << "(" << #cond << ")" << msg; \
	} 
	//Q_ASSERT_X(cond, QF_FUNC_NAME, QFString(what).str());
	/*
#define QF_INTERNAL_ERROR_ASSERT(cond, what) \
	qfFat("ASSERT") << QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME) << what; \
	Q_ASSERT_X(cond, QF_FUNC_NAME, ("INTERNAL ERROR" + QFString(what)).str());
	*/

#endif
