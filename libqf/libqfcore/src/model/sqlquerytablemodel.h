#ifndef QF_CORE_MODEL_SQLQUERYTABLEMODEL_H
#define QF_CORE_MODEL_SQLQUERYTABLEMODEL_H

#include "tablemodel.h"
#include "../sql/querybuilder.h"
#include "../sql/query.h"

#include <QMap>
#include <QString>

namespace qf {
namespace core {
namespace sql {
class Connection;
}
namespace model {

class QFCORE_DECL_EXPORT SqlQueryTableModel : public TableModel
{
	Q_OBJECT
	Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
	Q_PROPERTY(QVariantMap queryParameters READ queryParameters WRITE setQueryParameters)
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName)
private:
	typedef TableModel Super;
public:
	SqlQueryTableModel(QObject *parent = 0);
public:
	bool reload(const QString &query_str);
	bool reload() Q_DECL_OVERRIDE;
	bool postRow(int row_no, bool throw_exc) Q_DECL_OVERRIDE;
	void revertRow(int row_no) Q_DECL_OVERRIDE;
public:
	void setQueryBuilder(const qf::core::sql::QueryBuilder &qb);

	QVariantMap queryParameters() const { return m_queryParameters; }
	void setQueryParameters(QVariantMap arg) { m_queryParameters = arg; }

	QString connectionName() const { return m_connectionName; }
	void setConnectionName(QString arg) { m_connectionName = arg; }

	QString query() const { return m_query; }
	void setQuery(QString arg) { if (m_query != arg) { m_query = arg; emit queryChanged(arg); } }
	Q_SIGNAL void queryChanged(QString arg);

	const qf::core::sql::Query& recentlyExecutedQuery() {return m_recentlyExecutedQuery;}

	void addForeignKeyDependency(const QString &master_table_key, const QString &slave_table_key);

protected:
	virtual QString buildQuery();
	virtual QString replaceQueryParameters(const QString query_str);
	qf::core::sql::Connection sqlConnection();
	bool reloadTable(const QString &query_str);
	QSet<QString> tableIds(const utils::Table::FieldList &table_fields);
	void setSqlFlags(qf::core::utils::Table::FieldList &table_fields, const QString &query_str);

	QSet<QString> referencedForeignTables();
	QStringList tableIdsSortedAccordingToForeignKeys();

	bool removeOneRow(int row_no, bool throw_exc = false) Q_DECL_OVERRIDE;
protected:
	qf::core::sql::QueryBuilder m_queryBuilder;
	QString m_query;
	QVariantMap m_queryParameters;
	QString m_connectionName;
	qf::core::sql::Query m_recentlyExecutedQuery;
	/// INSERT needs to know dependency of tables in joined queries to insert particular tables in proper order
	QMap<QString, QString> m_foreignKeyDependencies;
};

}}}

#endif // QF_CORE_MODEL_SQLQUERYTABLEMODEL_H
