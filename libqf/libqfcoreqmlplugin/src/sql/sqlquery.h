#ifndef QF_CORE_QML_SQLQUERY_H
#define QF_CORE_QML_SQLQUERY_H

#include <QObject>
#include <QSqlQuery>
#include <QVariant>

class QSqlDadabase;

namespace qf {
namespace core {
namespace qml {

class SqlRecord;

class SqlQuery : public QObject
{
	Q_OBJECT
public:
	explicit SqlQuery(QObject *parent = 0);

	void setDatabase(const QSqlDatabase &db);
public:
	Q_INVOKABLE bool exec(const QString &query_str = QString());
	Q_INVOKABLE bool isValid() {return m_query.isValid();}
	Q_INVOKABLE bool next() {return m_query.next();}
	Q_INVOKABLE QString lastError();
	Q_INVOKABLE QVariant value(int ix);
	Q_INVOKABLE QVariant value(const QString &field_name);
	Q_INVOKABLE QVariantList values();
	Q_INVOKABLE qf::core::qml::SqlRecord* record();
private:
	QSqlQuery m_query;
	SqlRecord *m_record;
};

}}}

#endif // QF_CORE_QML_SQLQUERY_H
