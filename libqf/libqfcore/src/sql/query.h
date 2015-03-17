#ifndef QF_CORE_SQL_QUERY_H
#define QF_CORE_SQL_QUERY_H

#include "../core/coreglobal.h"

#include <QSqlQuery>
#include <QSqlError>

class QSqlDatabase;

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT Query : public QSqlQuery
{
private:
	typedef QSqlQuery Super;
public:
	explicit Query(const QSqlDatabase &db = QSqlDatabase());
	explicit Query(const QString &connection_name);
public:
	//using Super::prepare;
	bool prepare(const QString& query, bool throw_exc = false);
	//using Super::exec;
	bool exec(const QString &query, bool throw_exc = false);
	bool exec(bool throw_exc = false);
	QSqlRecord record() const;
	int fieldIndex(const QString& field_name) const;
	using Super::value;
	QVariant value(const QString& field_name) const;
};

}}}

#endif // QF_CORE_SQL_QUERY_H
