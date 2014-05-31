/**
 * \file qflogcust.h including this file you will get rid of five debugging streams.
 */

#ifndef QF_LOGCUST_H
#define QF_LOGCUST_H

//#include <stdlib.h>
//#include <stdarg.h>
//#include <string.h>

//#include "qfcompat.h"
#include "qflog.h"

/*!
 * \def QF_LOG_NOT_MODULE_TRASH defining this one can disable logging of qfTrash()<br>
 * This macro must be introduced before #include<qflogcust.h>
 *
 * \def QF_LOG_MODULE_DOMAIN_NAME here you can change log domain for module<br>
 * This macro must be introduced before #include<qflogcust.h>
 */
#ifndef QF_LOG_MODULE_DOMAIN_NAME
#	define QF_LOG_MODULE_DOMAIN_NAME QLatin1String(__FILE__)
#endif

#define QF_LOG_DOMAIN_WITH_LINE_NO (QString("%1:%2").arg(QF_LOG_MODULE_DOMAIN_NAME).arg(__LINE__))

#define qfLog(level) QFLog(level, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfFatal() QFLog(QFLog::LOG_FATAL, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfError() QFLog(QFLog::LOG_ERR, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfWarning() QFLog(QFLog::LOG_WARN, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfInfo() QFLog(QFLog::LOG_INF, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfDebug() QFLog(QFLog::LOG_DEB, QF_LOG_DOMAIN_WITH_LINE_NO)

//#ifdef QF_NO_TRASH_OUTPUT uz neni potreba, pouziva se allowedDomains() a denyiedDomains()
#ifndef QFLOG_DO_TRASH_LOG
#	define qfTrash() while(0) QFLog(QFLog::LOG_TRASH, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define qfTrashL(level) qfTrash()
#	define qfLogFuncFrame() qfTrash()
#else
#	define qfTrash() QFLog(QFLog::LOG_TRASH, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define qfTrashL(level) QFLog(QFLog::LOG_TRASH + level, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define qfLogFuncFrame() QFLog func_frame_exit_logger = qfTrash().pop() << "\tEXIT" << QF_FUNC_NAME; \
			qfTrash().push() << QF_FUNC_NAME
#endif

#define logFuncFrame() QFLog func_frame_exit_logger = logTrash().pop() << "\tEXIT" << QF_FUNC_NAME; \
			logTrash().push() << QF_FUNC_NAME

#define logScope(level) QFLog func_frame_exit_logger = log(level).pop(); \
			log(level).push()
#define logScopeTo(logger, level) QFLog func_frame_exit_logger = logger->log(level).pop(); \
			logger->log(level).push()

//#define SCOPE_EXIT_INFO_LOGGER(object) QFLog scope_exit_logger = object->logInfo().pop()

//====================== logging support =======================

/*
 * \def QF_LOG_NOT_MODULE_TRASH defining this one can disable logging of qfTrash()<br>
 * This macro must be introduced before #include<qflogcust.h>
 */
 /*
#ifdef QF_LOG_NOT_MODULE_TRASH
#	define QF_TRASH_LOG(domain_name) static inline QFLog qfTrash() {return QFLog(QFLog::LOG_NULL, domain_name);}
#else
#	define QF_TRASH_LOG(domain_name) static inline QFLog qfTrash() {return QFLog(QFLog::LOG_TRASH, domain_name);}
#endif

#define QF_DECLARE_CUSTOM_LOG2(domain_name) \
static inline QFLog qfLog(int level) {return QFLog(level, domain_name);} \
static inline QFLog qfFatal() {return QFLog(QFLog::LOG_FATAL, domain_name);} \
static inline QFLog qfError() {return QFLog(QFLog::LOG_ERR, domain_name);} \
static inline QFLog qfWarning() {return QFLog(QFLog::LOG_WARN, domain_name);} \
static inline QFLog qfInfo() {return QFLog(QFLog::LOG_INF, domain_name);} \
static inline QFLog qfDebug() {return QFLog(QFLog::LOG_DEB, domain_name);} \
QF_TRASH_LOG(domain_name)

#define QF_DECLARE_CUSTOM_LOG QF_DECLARE_CUSTOM_LOG2(QF_LOG_MODULE_DOMAIN_NAME)
 */
//=============================================

#endif // QF_LOGCUST_H

