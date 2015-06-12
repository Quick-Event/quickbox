#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include "coreglobal.h"

#include <QDebug>

#define qfDebug qDebug
#define qfInfo() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(qf::core::Log::Level::Info))
#define qfWarning qWarning
#define qfError qCritical
#define qfFatal if(qCritical() << qf::core::Log::stackTrace(), true) qFatal
#define qfLog(level) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(level))

#define qfLogFuncFrame() QDebug __func_frame_exit_logger__ = QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug() << "     EXIT FN" << Q_FUNC_INFO; \
	qfDebug() << ">>>> ENTER FN" << Q_FUNC_INFO

class QLoggingCategory;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Log
{
public:
	enum class Level : int { Invalid = -1, Fatal, Error, Warning, Info, Debug };
public:
	static const char* categoryDebugName;
	static const char* categoryInfoName;
	static const char* categoryWarningName;
	static const char* categoryErrorName;
	static const char* categoryFatalName;

	static const QLoggingCategory& categoryForLevel(Level level);
	static const char *levelName(Level level);

	static void checkLogLevelMetaTypeRegistered();
public:
	static QString stackTrace();
};

}
}

Q_DECLARE_METATYPE(qf::core::Log::Level)

#endif
