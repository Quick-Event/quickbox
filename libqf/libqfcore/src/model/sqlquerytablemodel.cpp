#include "sqlquerytablemodel.h"
#include "../core/assert.h"

#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlIndex>
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

bool SqlQueryTableModel::postRow(int row_no)
{
	qfLogFuncFrame() << row_no;
	return true;
}

void SqlQueryTableModel::revertRow(int row_no)
{
	qfLogFuncFrame() << row_no;
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
	setSqlFlags(table_fields);
	m_table = qfu::Table(table_fields);
	while(q.next()) {
		qfu::TableRow &row = m_table.appendRow();
		for(int i=0; i<fld_cnt; i++) {
			row.setBareBoneValue(i, q.value(i));
		}
	}
}

static QString compose_table_id(const QString &table_name, const QString &schema_name)
{
	QString ret = table_name;
	if(!schema_name.isEmpty())
		ret = schema_name + '.' + ret;
	return ret;
}

void SqlQueryTableModel::setSqlFlags(qf::core::utils::Table::FieldList &table_fields)
{
	QSet<QString> table_ids;
	QSet<QString> field_ids;
	int fld_cnt = table_fields.count();
	for(int i=0; i<fld_cnt; i++) {
		const qfu::Table::Field &fld = table_fields[i];
		QString fs, ts, ds;
		qf::core::Utils::parseFieldName(fld.name(), &fs, &ts, &ds);
		ts = compose_table_id(ts, ds);
		if(!ts.isEmpty())
			table_ids << ts;
		field_ids << fs;
	}
	QSet<QString> updateable_table_ids;
	for(QString table_id : table_ids) {
		QStringList prikey_fields = primaryIndex(table_id);
		bool ok = true;
		for(auto pk_f : prikey_fields) {
			if(!field_ids.contains(pk_f)) {
				ok = false;
				break;
			}
		}
		if(ok) {
			updateable_table_ids << table_id;
		}
	}
	for(int i=0; i<fld_cnt; i++) {
		qfu::Table::Field fld = table_fields[i];
		QString fs, ts, ds;
		qf::core::Utils::parseFieldName(fld.name(), &fs, &ts, &ds);
		QString table_id = compose_table_id(ts, ds);
		fld.setCanUpdate(updateable_table_ids.contains(table_id));
		QStringList prikey_fields = primaryIndex(table_id);
		fld.setPriKey(prikey_fields.contains(fs));
	}
}

QStringList SqlQueryTableModel::primaryIndex(const QString &table_name)
{
	QString pk_key = connectionName() + '.' + table_name;
	QStringList ret;
	if(!m_primaryIndexCache.contains(pk_key)) {
		QSqlDatabase db = QSqlDatabase::database(connectionName());
		QSqlIndex sql_ix = db.primaryIndex(table_name);
		for(int i=0; i<sql_ix.count(); i++) {
			QString fld_name = sql_ix.fieldName(i);
			qf::core::Utils::parseFieldName(fld_name, &fld_name);
			ret << fld_name;
		}
		m_primaryIndexCache[pk_key] = ret;
	}
	else {
		ret = m_primaryIndexCache.value(pk_key);
	}
	return ret;
}
