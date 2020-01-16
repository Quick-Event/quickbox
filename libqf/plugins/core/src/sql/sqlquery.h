#ifndef QF_CORE_QML_SQLQUERY_H
#define QF_CORE_QML_SQLQUERY_H

#include <qf/core/sql/query.h>

#include <QObject>
#include <QSqlQuery>
#include <QVariant>
#include <QPointer>

class QSqlDadabase;

namespace qf {
namespace core {
namespace qml {

class SqlRecord;
class SqlQueryBuilder;

class SqlQuery : public QObject
{
	Q_OBJECT
	Q_ENUMS(ParamType)
public:
	enum ParamType { In = QSql::In, Out = QSql::Out, InOut = QSql::InOut, Binary = QSql::Binary };
public:
	explicit SqlQuery(QObject *parent = nullptr);
	~SqlQuery() Q_DECL_OVERRIDE;

	void setDatabase(const QSqlDatabase &db);
public:
	Q_INVOKABLE bool exec();
	Q_INVOKABLE bool exec(const QString &query_str);
	Q_INVOKABLE bool exec(qf::core::qml::SqlQueryBuilder *qb);
	Q_INVOKABLE bool isValid() {return m_query.isValid();}
	Q_INVOKABLE bool next() {return m_query.next();}
	Q_INVOKABLE QString lastError();
	Q_INVOKABLE QVariant lastInsertId();
	Q_INVOKABLE int numRowsAffected();
	Q_INVOKABLE QVariant value(int ix);
	Q_INVOKABLE QVariant value(const QString &field_name);
	//Q_INVOKABLE QVariantList values();
	Q_INVOKABLE qf::core::qml::SqlRecord* record();
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* createBuilder();

	Q_INVOKABLE bool prepare(const QString &query_str);
	Q_INVOKABLE void bindValue(const QString &placeholder, const QVariant &val, int param_type = ParamType::In);
private:
	qf::core::sql::Query m_query;
	SqlRecord *m_record;
	//QPointer<SqlQueryBuilder> m_queryBuilder;
};

}}}

#endif // QF_CORE_QML_SQLQUERY_H
