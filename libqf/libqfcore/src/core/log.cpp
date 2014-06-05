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
	static const char *names[] = {"DEB", "INFO", "WARN", "ERR", "FATAL"};
	static int cnt = sizeof(names) / sizeof(char*);
	if(level >= 0 && level < cnt)
		return names[level];
	return "???";
}

QString Log::stackTrace()
{
	return StackTrace::stackTrace().toString();
}

