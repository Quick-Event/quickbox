#include "log.h"
#include "stacktrace.h"

#include <QLoggingCategory>

using namespace qf::core;

const char* Log::categoryDebugName = "QF_CATEGORY_DEBUG";
const char* Log::categoryInfoName = "QF_CATEGORY_INFO";
const char* Log::categoryWarningName = "QF_CATEGORY_WARNING";
const char* Log::categoryErrorName = "QF_CATEGORY_ERROR";
const char* Log::categoryFatalName = "QF_CATEGORY_FATAL";

const QLoggingCategory &Log::categoryForLevel(int level)
{
	static QLoggingCategory catDebug(categoryDebugName);
	static QLoggingCategory catInfo(categoryInfoName);
	static QLoggingCategory catWarning(categoryWarningName);
	static QLoggingCategory catError(categoryErrorName);
	static QLoggingCategory catFatal(categoryFatalName);
	switch(level) {
		case LOG_DEB: return catDebug;
		case LOG_INFO: return catInfo;
		case LOG_WARN: return catWarning;
		case LOG_ERR: return catError;
		case LOG_FATAL: return catFatal;
	}
	return catDebug;
}

const char *qf::core::Log::levelName(qf::core::Log::Level level)
{
	switch(level) {
	case qf::core::Log::LOG_FATAL:
		return "FATAL";
	case qf::core::Log::LOG_ERR:
		return "ERR";
	case qf::core::Log::LOG_WARN:
		return "WARN";
	case qf::core::Log::LOG_INFO:
		return "INFO";
	case qf::core::Log::LOG_DEB:
		return "DEB";
	case qf::core::Log::LOG_INVALID:
		return "INVALID";
	}
	return "???";
}

QString Log::stackTrace()
{
	return StackTrace::stackTrace().toString();
}

