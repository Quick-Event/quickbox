#include "logcore.h"
#include "stacktrace.h"

#include <QLoggingCategory>

using namespace qf::core;
/*
const char* Log::categoryDebugName = "QF_CATEGORY_DEBUG";
const char* Log::categoryInfoName = "QF_CATEGORY_INFO";
const char* Log::categoryWarningName = "QF_CATEGORY_WARNING";
const char* Log::categoryErrorName = "QF_CATEGORY_ERROR";
const char* Log::categoryFatalName = "QF_CATEGORY_FATAL";

const QLoggingCategory &Log::categoryForLevel(Level level)
{
	static QLoggingCategory catDebug(categoryDebugName);
	static QLoggingCategory catInfo(categoryInfoName);
	static QLoggingCategory catWarning(categoryWarningName);
	static QLoggingCategory catError(categoryErrorName);
	static QLoggingCategory catFatal(categoryFatalName);
	switch(Log::Level(level)) {
		case Log::Level::Debug: return catDebug;
		case Log::Level::Info: return catInfo;
		case Log::Level::Warning: return catWarning;
		case Log::Level::Error: return catError;
		case Log::Level::Fatal:
		default: return catFatal;
	}
	return catDebug;
}
*/
const char *qf::core::Log::levelName(qf::core::Log::Level level)
{
	switch(level) {
	case Log::Level::Fatal:
		return "FATAL";
	case Log::Level::Error:
		return "ERR";
	case Log::Level::Warning:
		return "WARN";
	case Log::Level::Info:
		return "INFO";
	case Log::Level::Debug:
		return "DEB";
	case Log::Level::Invalid:
		return "INVALID";
	}
	return "???";
}

QString Log::levelToString(Log::Level level)
{
	switch(level) {
	case Log::Level::Fatal:
		return QStringLiteral("Fatal");
	case Log::Level::Error:
		return QStringLiteral("Error");
	case Log::Level::Warning:
		return QStringLiteral("Warning");
	case Log::Level::Info:
		return QStringLiteral("Info");
	case Log::Level::Debug:
		return QStringLiteral("Debug");
	case Log::Level::Invalid:
		return QStringLiteral("Invalid");
	}
	return QString();
}

void Log::checkLogLevelMetaTypeRegistered()
{
	int id = qMetaTypeId<qf::core::Log::Level>();
	if(!QMetaType::isRegistered(id)) {
		qRegisterMetaType<qf::core::Log::Level>();
	}
}

QString Log::stackTrace()
{
	return StackTrace::stackTrace().toString();
}

