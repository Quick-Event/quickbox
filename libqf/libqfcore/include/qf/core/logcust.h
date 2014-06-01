/**
 * \file Logcust.h including this file you will get rid of five debugging streams.
 */

#ifndef QF_CORE_LOGCUST_H
#define QF_CORE_LOGCUST_H

#include <qf/core/log.h>

/*!
 * \def QF_LOG_NOT_MODULE_TRASH defining this one can disable logging of qfTrash()<br>
 * This macro must be introduced before #include<Logcust.h>
 *
 * \def QF_LOG_MODULE_DOMAIN_NAME here you can change log domain for module<br>
 * This macro must be introduced before #include<Logcust.h>
 */
#ifndef QF_LOG_MODULE_DOMAIN_NAME
#	define QF_LOG_MODULE_DOMAIN_NAME QLatin1String(__FILE__)
#endif

#define QF_LOG_DOMAIN_WITH_LINE_NO (QString("%1:%2").arg(QF_LOG_MODULE_DOMAIN_NAME).arg(__LINE__))

#define qfLog(level) qf::core::Log(level, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfFatal() qf::core::Log(qf::core::Log::LOG_FATAL, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfError() qf::core::Log(qf::core::Log::LOG_ERR, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfWarning() qf::core::Log(qf::core::Log::LOG_WARN, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfInfo() qf::core::Log(qf::core::Log::LOG_INF, QF_LOG_DOMAIN_WITH_LINE_NO)
#define qfDebug() qf::core::Log(qf::core::Log::LOG_DEB, QF_LOG_DOMAIN_WITH_LINE_NO)

//#ifdef QF_NO_TRASH_OUTPUT uz neni potreba, pouziva se allowedDomains() a denyiedDomains()
#ifndef Log_DO_TRASH_LOG
#	define qfTrash() while(0) qf::core::Log(qf::core::Log::LOG_TRASH, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define qfTrashL(level) qfTrash()
#	define LogFuncFrame() qfTrash()
#else
#	define qfTrash() qf::core::Log(qf::core::Log::LOG_TRASH, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define qfTrashL(level) qf::core::Log(qf::core::Log::LOG_TRASH + level, QF_LOG_DOMAIN_WITH_LINE_NO)
#	define LogFuncFrame() qf::core::Log func_frame_exit_logger = qfTrash().pop() << "\tEXIT" << QF_FUNC_NAME; \
			qfTrash().push() << QF_FUNC_NAME
#endif

#define qfLogFuncFrame() qf::core::Log func_frame_exit_logger = qfTrash().pop() << "\tEXIT" << QF_FUNC_NAME; \
			qfTrash().push() << QF_FUNC_NAME

#define qfLogScope(level) qf::core::Log func_frame_exit_logger = qf::core::log(level).pop(); \
			qf::core::log(level).push()
#define qfLogScopeTo(logger, level) qf::core::Log func_frame_exit_logger = logger->log(level).pop(); \
			logger->log(level).push()

//#define SCOPE_EXIT_INFO_LOGGER(object) qf::core::Log scope_exit_logger = object->logInfo().pop()

//====================== logging support =======================

/*
 * \def QF_LOG_NOT_MODULE_TRASH defining this one can disable logging of qfTrash()<br>
 * This macro must be introduced before #include<Logcust.h>
 */
 /*
#ifdef QF_LOG_NOT_MODULE_TRASH
#	define QF_TRASH_LOG(domain_name) static inline qf::core::Log qfTrash() {return qf::core::Log(qf::core::Log::LOG_NULL, domain_name);}
#else
#	define QF_TRASH_LOG(domain_name) static inline qf::core::Log qfTrash() {return qf::core::Log(qf::core::Log::LOG_TRASH, domain_name);}
#endif

#define QF_DECLARE_CUSTOM_LOG2(domain_name) \
static inline qf::core::Log qf::core::Log(int level) {return qf::core::Log(level, domain_name);} \
static inline qf::core::Log qfFatal() {return qf::core::Log(qf::core::Log::LOG_FATAL, domain_name);} \
static inline qf::core::Log qfError() {return qf::core::Log(qf::core::Log::LOG_ERR, domain_name);} \
static inline qf::core::Log qfWarning() {return qf::core::Log(qf::core::Log::LOG_WARN, domain_name);} \
static inline qf::core::Log qfInfo() {return qf::core::Log(qf::core::Log::LOG_INF, domain_name);} \
static inline qf::core::Log qfDebug() {return qf::core::Log(qf::core::Log::LOG_DEB, domain_name);} \
QF_TRASH_LOG(domain_name)

#define QF_DECLARE_CUSTOM_LOG QF_DECLARE_CUSTOM_LOG2(QF_LOG_MODULE_DOMAIN_NAME)
 */
//=============================================

#endif // QF_CORE_LOGCUST_H

