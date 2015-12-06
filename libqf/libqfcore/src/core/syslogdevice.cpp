#include "syslogdevice.h"

#ifdef Q_OS_UNIX
#include <syslog.h>
#endif

namespace qf {
namespace core {

SysLogDevice::SysLogDevice(QObject *parent)
	: Super(parent)
{
}

SysLogDevice::~SysLogDevice()
{
}

SysLogDevice *SysLogDevice::install()
{
	auto *ret = new SysLogDevice();
	Super::install(ret);
	return ret;
}

void SysLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
#ifdef Q_OS_UNIX
	int syslog_level;
	switch (level) {
	case Log::Level::Debug: syslog_level = LOG_DEBUG; break;
	case Log::Level::Info: syslog_level = LOG_INFO; break;
	case Log::Level::Warning: syslog_level = LOG_WARNING; break;
	case Log::Level::Error: syslog_level = LOG_CRIT; break;
	default: syslog_level = LOG_EMERG; break;
	}
	QString module = moduleFromContext(context);
	syslog(syslog_level, "<%s>[%s:%d] %s", Log::levelName(level), qPrintable(module), context.line, qPrintable(msg));
	//LogEntryMap m(level, domain, msg, context.file, context.line, context.function);
#else
	Q_UNUSED(level)
	Q_UNUSED(context)
	Q_UNUSED(msg)
#warning syslog is supported on Unix platforms only

#endif
}

} // namespace core
} // namespace qf

