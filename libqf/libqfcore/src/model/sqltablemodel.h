#ifndef QF_CORE_MODEL_SQLTABLEMODEL_H
#define QF_CORE_MODEL_SQLTABLEMODEL_H

#include "tablemodel.h"
#include "../core/utils.h"
#include "../sql/querybuilder.h"
#include "../sql/query.h"
#include "../sql/dbenumcache.h"

#include <QMap>
#include <QString>

namespace qf {
namespace core {
namespace sql {
class Connection;
}
namespace model {

class QFCORE_DECL_EXPORT SqlTableModel : public TableModel
{
	Q_OBJECT
	Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
	//Q_PROPERTY(QVariant queryParameters READ queryParameters WRITE setQueryParameters NOTIFY queryParametersChanged)
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
private:
	typedef TableModel Super;
public:
	SqlTableModel(QObject *parent = nullptr);
	~SqlTableModel() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QVariant, q, Q, ueryParameters)
	QF_PROPERTY_BOOL_IMPL(i, I, ncludeJoinedTablesIdsToReloadRowQuery)

public:
	class QFCORE_DECL_EXPORT DbEnumCastProperties : public QVariantMap
	{
		QF_VARIANTMAP_FIELD(QString, g, setG, roupName)
		QF_VARIANTMAP_FIELD2(QString, c, setC, aptionFormat, QStringLiteral("{{caption}}"))

	public:
		DbEnumCastProperties(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}
};
public:
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	Q_INVOKABLE QString effectiveQuery();
	bool reload() Q_DECL_OVERRIDE;
	bool postRow(int row_no, bool throw_exc) Q_DECL_OVERRIDE;
	void revertRow(int row_no) Q_DECL_OVERRIDE;
	int reloadRow(int row_no) Q_DECL_OVERRIDE;
	int reloadInserts(const QString &id_column_name) Q_DECL_OVERRIDE;
public:
	void setQueryBuilder(const qf::core::sql::QueryBuilder &qb, bool clear_columns = false);
	const qf::core::sql::QueryBuilder& queryBuilder() const;

	QString connectionName() const { return m_connectionName; }
	void setConnectionName(QString arg)
	{
		if(m_connectionName != arg) {
			m_connectionName = arg;
			emit connectionNameChanged(arg);
		}
	}
	Q_SIGNAL void connectionNameChanged(QString arg);

	qf::core::sql::Connection sqlConnection();
	QString query() const { return m_query; }
	void setQuery(const QString &query_str);
	Q_SIGNAL void queryChanged(const QString &query_str);

	const qf::core::sql::Query& recentlyExecutedQuery() {return m_recentlyExecutedQuery;}
	Q_INVOKABLE const QString& recentlyExecutedQueryString() const {return m_recentlyExecutedQueryString;}

	void addForeignKeyDependency(const QString &master_table_key, const QString &slave_table_key);
protected:
	virtual QString buildQuery();
	virtual QString replaceQueryParameters(const QString query_str);

	bool reloadQuery(const QString &query_str);

	virtual bool reloadTable(const QString &query_str);
	QStringList tableIds(const utils::Table::FieldList &table_fields);
	void setSqlFlags(qf::core::utils::Table::FieldList &table_fields, const QString &query_str);

	QSet<QString> referencedForeignTables();
	QStringList tableIdsSortedAccordingToForeignKeys();

	bool removeTableRow(int row_no, bool throw_exc = false) Q_DECL_OVERRIDE;
protected:
	qf::core::sql::QueryBuilder m_queryBuilder;
	QString m_query;
	QString m_connectionName;
	qf::core::sql::Query m_recentlyExecutedQuery;
	QString m_recentlyExecutedQueryString;
	/// INSERT needs to know dependency of tables in joined queries to insert particular tables in proper order
	QMap<QString, QString> m_foreignKeyDependencies;
};

}}}

#endif // QF_CORE_MODEL_SQLTABLEMODEL_H
