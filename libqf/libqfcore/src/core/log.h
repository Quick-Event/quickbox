#pragma once

#include "string.h"
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QDateTime>
#include <QSize>

#define QF_LOG_NECROLOG

#ifdef QT_NO_DEBUG
#define QF_NO_DEBUG_LOG
#endif

#ifdef QF_NO_DEBUG_LOG
#define NECROLOG_NO_DEBUG_LOG
#endif

#include <necrolog.h>

#define qfCDebug(category) nCDebug(category)
#define qfCMessage(category) nCMessage(category)
#define qfCInfo(category) nCInfo(category)
#define qfCWarning(category) nCWarning(category)
#define qfCError(category) nCError(category)

#define qfLogFuncFrame() nLogFuncFrame()

#define qfDebug() qfCDebug("")
#define qfMessage() qfCMessage("")
#define qfInfo() qfCInfo("")
#define qfWarning() qfCWarning("")
#define qfError() qfCError("")

#define qfFatal(msg) {qfError() << msg; abort();}

inline NecroLog &operator<<(NecroLog log, const QString &s) { return log.operator<<(s.toStdString()); }
inline NecroLog &operator<<(NecroLog log, const QDateTime &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
inline NecroLog &operator<<(NecroLog log, const QDate &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
inline NecroLog &operator<<(NecroLog log, const QTime &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
inline NecroLog &operator<<(NecroLog log, const QStringList &sl) {
	QString s = '[' + sl.join(',') + ']';
	return log.operator<<(s.toStdString());
}
inline NecroLog &operator<<(NecroLog log, const QByteArray &ba) {
	QString s = ba.toHex();
	return log.operator<<(s.toStdString());
}
inline NecroLog &operator<<(NecroLog log, const QVariant &v) {
	QString s = v.toString();
	return log.operator<<(s.toStdString());
}
inline NecroLog &operator<<(NecroLog log, const qf::core::String &s) { return log.operator<<(s.toStdString()); }
inline NecroLog &operator<<(NecroLog log, const QSize &sz) {
	QString s = "QSize(%1, %2)";
	return log.operator<<(s.arg(sz.width()).arg(sz.height()).toStdString());
}
Q_DECLARE_METATYPE(NecroLog::Level)
