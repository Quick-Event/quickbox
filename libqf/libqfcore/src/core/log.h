#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include <QDebug>

#define qfDebug qDebug
#define qfInfo() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(qf::core::Log::LOG_INFO))
#define qfWarning qWarning
#define qfError qCritical
#define qfFatal qFatal
#define qfLog(level) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(level))

#define qfLogFuncFrame() QDebug __func_frame_exit_logger__ = QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug() << "<< FN EXIT" << Q_FUNC_INFO; \
	qfDebug() << ">> FN ENTER" << Q_FUNC_INFO

class QLoggingCategory;

namespace qf {
namespace core {

class Log
{
public:
	enum Level {LOG_DEB, LOG_INFO, LOG_WARN, LOG_ERR, LOG_FATAL};
public:
	static const char* categoryDebugName;
	static const char* categoryInfoName;
	static const char* categoryWarningName;
	static const char* categoryErrorName;
	static const char* categoryFatalName;

	static const QLoggingCategory& categoryForLevel(int level);
	static const char *levelName(Level level);
public:
	static QString stackTrace();
};

}
}

#endif			
