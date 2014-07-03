#ifndef QF_CORE_QML_SQLQUERY_H
#define QF_CORE_QML_SQLQUERY_H

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
public:
	explicit SqlQuery(QObject *parent = 0);
	~SqlQuery() Q_DECL_OVERRIDE;

	void setDatabase(const QSqlDatabase &db);
public:
	Q_INVOKABLE bool exec(const QString &query_str = QString());
	Q_INVOKABLE bool exec(qf::core::qml::SqlQueryBuilder *qb);
	Q_INVOKABLE bool isValid() {return m_query.isValid();}
	Q_INVOKABLE bool next() {return m_query.next();}
	Q_INVOKABLE QString lastError();
	Q_INVOKABLE QVariant value(int ix);
	Q_INVOKABLE QVariant value(const QString &field_name);
	Q_INVOKABLE QVariantList values();
	Q_INVOKABLE qf::core::qml::SqlRecord* record();
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* builder();
private:
	QSqlQuery m_query;
	SqlRecord *m_record;
	QPointer<SqlQueryBuilder> m_queryBuilder;
};

}}}

#endif // QF_CORE_QML_SQLQUERY_H
