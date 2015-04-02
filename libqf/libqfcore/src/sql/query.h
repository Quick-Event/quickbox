#ifndef QF_CORE_SQL_QUERY_H
#define QF_CORE_SQL_QUERY_H

#include "../core/coreglobal.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>

class QSqlDatabase;

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT Query : public QSqlQuery
{
private:
	typedef QSqlQuery Super;
public:
	/// If db is invalid, the application's default database will be used.
	explicit Query(const QSqlDatabase &db = QSqlDatabase());
	explicit Query(const QString &connection_name);
public:
	//using Super::prepare;
	bool prepare(const QString& query, bool throw_exc = false);
	//using Super::exec;
	bool exec(const QString &query, bool throw_exc = false);
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
};

}}}

#endif // QF_CORE_SQL_QUERY_H
