#ifndef QF_CORE_SQL_QUERY_H
#define QF_CORE_SQL_QUERY_H

#include "../core/coreglobal.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>

class QSqlDatabase;

namespace qf {
namespace core {
namespace sql {

class QueryBuilder;

class QFCORE_DECL_EXPORT Query : public QSqlQuery
{
private:
	typedef QSqlQuery Super;
public:
	explicit Query(const QSqlDatabase &db);
	/// If connection_name is empty, the application's default database will be used.
	explicit Query(const QString &connection_name = QString());
public:
	//using Super::prepare;
	bool prepare(const QString& query, bool throw_exc = false);
	//using Super::exec;
	static Query fromExec(const QString &query, bool throw_exc = true);
	bool exec(const QString &query, bool throw_exc = false);
	bool execThrow(const QString &query) {return exec(query, true);}
	bool exec(const QueryBuilder &query_builder, bool throw_exc = false);
	/// necessary for proper overloading, const char* is treated as bool without this function
	bool exec(const char *query, bool throw_exc = false) {return exec(QString::fromUtf8(query), throw_exc);}
	bool exec(bool throw_exc = false);
	bool execCommands(const QStringList &commands, const QMap<QString, QString> &replacements = QMap<QString, QString>());
	void execCommandsThrow(const QStringList &commands, const QMap<QString, QString> &replacements = QMap<QString, QString>());
	QSqlRecord record() const;
	int fieldIndex(const QString& field_name) const;
	using Super::value;
	QVariant value(const QString& field_name) const;
	QVariantMap values() const;
	QString lastErrorText() const;
private:
	mutable QSqlRecord m_demangledRecord;
};

QFCORE_DECL_EXPORT QVariantMap recordToMap(const QSqlRecord &rec);
QFCORE_DECL_EXPORT QVariantMap recordDiff(const QVariantMap &from, const QVariantMap &to);

}}}

#endif // QF_CORE_SQL_QUERY_H
