#include "sqlquerytablemodel.h"
#include "../core/assert.h"

#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

namespace qfs = qf::core::sql;
namespace qfu = qf::core::utils;
using namespace qf::core::model;

SqlQueryTableModel::SqlQueryTableModel(QObject *parent)
	: Super(parent)
{
	m_connectionName = QSqlDatabase::defaultConnection;
}

void SqlQueryTableModel::setQueryBuilder(const qf::core::sql::QueryBuilder &qb)
{
	m_queryBuilder = qb;
}

void SqlQueryTableModel::reload()
{
	QString qs = buildQuery();
	qs = replaceQueryParameters(qs);
	beginResetModel();
	reloadTable(qs);
	if(m_columns.isEmpty() || m_autoColumns)
		createColumnsFromTableFields();
	fillColumnIndexes();
	endResetModel();
}

QString SqlQueryTableModel::buildQuery()
{
	QString ret = m_query;
	if(ret.isEmpty()) {
		ret = m_queryBuilder.toString();
	}
	return ret;
}

QString SqlQueryTableModel::replaceQueryParameters(const QString query_str)
{
	QString ret = query_str;
	QMapIterator<QString, QVariant> it(m_queryParameters);
	while(it.hasNext()) {
		it.next();
		QString key = "${" + it.key() + "}";
		ret.replace(key, it.value().toString());
	}
	return ret;
}

void SqlQueryTableModel::reloadTable(const QString &query_str)
{
	QSqlDatabase db = QSqlDatabase::database(connectionName());
	QSqlQuery q(db);
	bool ok = q.exec(query_str);
	QF_ASSERT(ok == true,
			  QString("SQL Error: %1\n%2").arg(q.lastError().text()).arg(query_str),
			  return);
	qfu::Table::FieldList table_fields;
	QSqlRecord rec = q.record();
	int fld_cnt = rec.count();
	for(int i=0; i<fld_cnt; i++) {
		QSqlField rec_fld = rec.field(i);
		qfu::Table::Field fld(rec_fld.name(), rec_fld.type());
		table_fields << fld;
	}
	m_table = qfu::Table(table_fields);
	while(q.next()) {
		qfu::TableRow &row = m_table.appendRow();
		for(int i=0; i<fld_cnt; i++) {
			row.setInitialValue(i, q.value(i));
		}
	}
}
