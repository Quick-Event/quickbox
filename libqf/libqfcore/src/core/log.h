#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include <QDebug>

#define qfLogFuncFrame() QMessageLogger __func_frame_exit_logger__(__FILE__, __LINE__, Q_FUNC_INFO); \
	__func_frame_exit_logger__.debug() << "\tFN EXIT"; \
	qDebug() << "FN ENTER"

#define qfDebug qDebug
#define qfInfo() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryInfo())
#define qfWarning qWarning
#define qfError qCritical
#define qfFatal qFatal
#define qfLog(level) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(level))

class QLoggingCategory;

namespace qf {
namespace core {

class Log
{
public:
	enum Level {LOG_DEB, LOG_INFO, LOG_WARN, LOG_ERR, LOG_FATAL};
	static const char* categoryDebugName;
	static const char* categoryInfoName;
	static const char* categoryWarningName;
	static const char* categoryErrorName;
	static const char* categoryFatalName;

	static const QLoggingCategory& categoryForLevel(int level);
	static const char *levelName(Level level);

	static QString stackTrace();
	static const QLoggingCategory& categoryInfo();
private:
};

}
}

#endif			
