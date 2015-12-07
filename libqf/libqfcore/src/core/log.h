#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include "logcore.h"

#include <QDebug>

#ifndef QT_DEBUG
#define NO_QF_DEBUG
#endif
/*
#ifdef NO_QF_DEBUG
#define qfDebug_q while(0) qDebug
#else
#define qfDebug_q qDebug
#endif
#define qfInfo_q qInfo
#define qfWarning_q qWarning
#define qfError_q qCritical
#define qfFatal_q if(qCritical() << qf::core::Log::stackTrace(), true) qFatal

//#define qfLog_q(level) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning(qf::core::Log::categoryForLevel(level))

#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 0)) || (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
#define qfDebug qfDebug_q()
#define qfInfo qfInfo_q()
#define qfWarning qfWarning_q()
#define qfError qfError_q()
#define qfFatal qfFatal_q
//#define qfLog(level) qfLog_q(level)
#else
#define qfDebug qfDebug_q().noquote
#define qfInfo qfInfo_q().noquote
#define qfWarning qfWarning_q().noquote
#define qfError qfError_q().noquote
#define qfFatal qfFatal_q
//#define qfLog(level) qfLog_q(level).noquote()
#endif
*/

#define qfFatal if(qCritical() << qf::core::Log::stackTrace(), true) qFatal

#ifdef NO_QF_DEBUG
#define qfCDebug(category) while(0) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category).debug
#else
#define qfCDebug(category) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category).debug
#endif

#define qfCInfo(category) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category).info
#define qfCWarning(category) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category).warning
#define qfCError(category) QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category).critical

#define qfDebug qfCDebug("")
#define qfInfo qfCInfo("")
#define qfWarning qfCWarning("")
#define qfError qfCError("")

#ifdef NO_QF_DEBUG
#define qfLogFuncFrame() while(0) qDebug()
#else
#define qfLogFuncFrame() QDebug __func_frame_exit_logger__ = QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug() << "     EXIT FN" << Q_FUNC_INFO; \
	qfDebug() << ">>>> ENTER FN" << Q_FUNC_INFO
#endif

#endif
