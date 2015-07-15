#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include "coreglobal.h"

#include <QDebug>

#define qfDebug_q qDebug
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
#define qfInfo_q() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(qf::core::Log::Level::Info))
#else
#define qfInfo_q qInfo
#endif
#define qfWarning_q qWarning
#define qfError_q qCritical
#define qfFatal_q if(qCritical() << qf::core::Log::stackTrace(), true) qFatal

#define qfLog_q(level) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(level))

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
#define qfDebug qfDebug_q()
#define qfInfo qfInfo_q()
#define qfWarning qfWarning_q()
#define qfError qfError_q()
#define qfFatal qfFatal_q
#define qfLog qfLog_q
#else
#define qfDebug qfDebug_q().noquote
#define qfInfo qfInfo_q().noquote
#define qfWarning qfWarning_q().noquote
#define qfError qfError_q().noquote
#define qfFatal qfFatal_q
#define qfLog qfLog_q().noquote
#endif


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
