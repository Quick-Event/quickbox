#ifndef QF_CORE_LOGENTRYMAP_H
#define QF_CORE_LOGENTRYMAP_H

#include "coreglobal.h"
#include "necrolog.h"

#include <QVariantMap>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT LogEntryMap : public QVariantMap
{
public:
	LogEntryMap() : QVariantMap() {}
	LogEntryMap(NecroLog::Level level, const QString &category, const QString &message, const QString &file = QString(), int line = -1, const QString &function = QString());
	LogEntryMap(const QVariantMap &m) : QVariantMap(m) {}
public:
	NecroLog::Level level() const;
	LogEntryMap& setLevel(NecroLog::Level l);
	QString levelStr() const;
	QString category() const;
	LogEntryMap& setCategory(const QString &c);
	QString message() const;
	LogEntryMap& setMessage(const QString &m);
	QString file() const;
	LogEntryMap& setFile(const QString &f);
	int line() const;
	LogEntryMap& setLine(int l);
	QString function() const;
	LogEntryMap& setFunction(const QString &f);
	QDateTime timeStamp() const;
	LogEntryMap& setTimeStamp(const QDateTime &ts);

	virtual QString toString() const;
};

} // namespace core
} // namespace qf

Q_DECLARE_METATYPE(qf::core::LogEntryMap)

#endif // QF_CORE_LOGENTRYMAP_H
