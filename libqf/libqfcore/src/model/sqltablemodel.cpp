#include "sqltablemodel.h"
#include "../core/assert.h"
#include "../core/exception.h"
#include "../sql/connection.h"
//#include "../sql/query.h"

#include <QRegExp>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlField>
#include <QSqlDriver>

namespace qfs = qf::core::sql;
namespace qfu = qf::core::utils;
using namespace qf::core::model;

SqlTableModel::SqlTableModel(QObject *parent)
	: Super(parent)
{
	m_connectionName = QSqlDatabase::defaultConnection;
}

SqlTableModel::~SqlTableModel()
{

}

void SqlTableModel::setQueryBuilder(const qf::core::sql::QueryBuilder &qb)
{
	m_queryBuilder = qb;
}

const qf::core::sql::QueryBuilder &SqlTableModel::queryBuilder() const
{
	return m_queryBuilder;
}

void SqlTableModel::addForeignKeyDependency(const QString &master_table_key, const QString &slave_table_key)
{
	m_foreignKeyDependencies[master_table_key] = slave_table_key;
}

bool SqlTableModel::reload()
{
	QString qs = buildQuery();
	qs = replaceQueryParameters(qs);
	Super::reload();
	return reload(qs);
}

bool SqlTableModel::reload(const QString &query_str)
{
	beginResetModel();
	bool ok = reloadTable(query_str);
	if(ok) {
		if(m_columns.isEmpty() || m_autoColumns)
			createColumnsFromTableFields();
		fillColumnIndexes();
	}
	endResetModel();
	return ok;
}

bool SqlTableModel::postRow(int row_no, bool throw_exc)
{
	qfLogFuncFrame() << row_no;

	qfu::TableRow row = m_table.row(row_no);
	bool ret = true;
	if(row.isDirty()) {
		if(row.isInsert()) {
			qfDebug() << "\tINSERT";

			//QSet<QString> referenced_foreign_tables = referencedForeignTables();
			//int tbl_cnt = 0;
			qf::core::sql::Connection sql_conn = sqlConnection();
			QSqlDriver *sqldrv = sql_conn.driver();
			QStringList table_ids = tableIdsSortedAccordingToForeignKeys();
			for(QString table_id : table_ids) {
				qfDebug() << "\ttable:" << table_id;
				QSqlRecord rec;
				int i = -1;
				int serial_ix = -1;
				int primary_ix = -1;
				//QSqlIndex pri_ix = ti.primaryIndex();
				//bool has_blob_field = false;
				for(const qf::core::utils::Table::Field &fld : row.fields()) {
					i++;
					if(fld.tableId() != table_id)
						continue;
					//qfError() << ti.field(f.fieldName()).toString();
					bool is_field_dirty = row.isDirty(i);
					if(fld.isSerial()) {
						/// always include serial fields, an empty line cannot be inserted in other case
						is_field_dirty = true;
						serial_ix = i;
						qfDebug() << "\t serial ix:" << serial_ix;
					}
					if(fld.isPriKey()) {
						/// prikey field vzdy pridej do insertu, bez nej by to neslo (pokud je autoincrement, uplatni se i predchozi podminka)
						is_field_dirty = true;
						primary_ix = i;
					}
					if(!is_field_dirty)
						continue;
					qfDebug() << "\tdirty field:" << fld.name();
					QVariant v = row.value(i);
					if(!v.isNull() || fld.isSerial()) {
						/// null hodnotu nema smysl insertovat, pokud to neni nutne kvuli necemu jinemu
						/// naopak se pri insertu dokumentu z vice join tabulek muze stat, ze se vlozi to not-null fieldu null hodnota
						/// pokud je insert, join radek neexistuje, takze hodnoty jsou null a mysql v takovem pripade v resultsetu nastavi
						/// i not-null field na nullable
						//qfInfo() << "\t" << f.fieldName() << "value type:" << QVariant::typeToName(v.type());
						QSqlField new_fld;
						if(v.isNull() && fld.isSerial() && !fld.isNullable())
							v = 0;
						new_fld.setValue(v);
						//qfInfo() << "\t\t" << "val type:" << QVariant::typeToName(f.value().type());
						new_fld.setName(fld.shortName());
						rec.append(new_fld);
					}
				}
				if(!rec.isEmpty()) {
					qfDebug() << "updating table inserts" << table_id;
					qf::core::sql::Connection dbinfo(sql_conn);
					QString table = dbinfo.fullTableNameToQtDriverTableName(table_id);
					QString s = sqldrv->sqlStatement(QSqlDriver::InsertStatement, table, rec, true);
					qfs::Query q(sql_conn);
					bool ok = q.prepare(s);
					if(!ok) {
						qfError() << "Cannot prepare query:" << s;
					}
					else {
						for(int i=0; i<rec.count(); i++) {
							QVariant::Type type = rec.field(i).value().type();
							//qfInfo() << "\t" << rec.field(i).name() << "bound type:" << QVariant::typeToName(type);
							qfDebug() << "\t\t" << rec.field(i).name() << "bound type:" << QVariant::typeToName(type) << "value:" << rec.field(i).value().toString().mid(0, 100);
							q.addBindValue(rec.field(i).value());
						}
						qfDebug() << "\texecuting prepared query:" << s;
						ok = q.exec();
						if(!ok) {
							qfError() << "Error executing query:" << s
									  << "\n" << q.lastError().text();
						}
						else {
							qfDebug() << "\tnum rows affected:" << q.numRowsAffected();
							int num_rows_affected = q.numRowsAffected();
							//setNumRowsAffected(q.numRowsAffected());
							QF_ASSERT(num_rows_affected == 1,
									  tr("numRowsAffected() = %1, should be 1\n%2").arg(num_rows_affected).arg(s),
									  return false);
							if(serial_ix >= 0) {
								QVariant v = q.lastInsertId();
								row.setValue(serial_ix, v);
								row.setDirty(serial_ix, false);
							}
						}
					}
				}
				if(primary_ix >= 0) {
					/// update foreign keys in the slave tables
					qf::core::utils::Table::Field pri_ix_fld = row.fields().value(primary_ix);
					QString master_key = pri_ix_fld.name();
					qfDebug() << "\t master_key:" << master_key;
					QString slave_key = m_foreignKeyDependencies.value(master_key);
					if(!slave_key.isEmpty()) {
						qfDebug() << "\tsetting value of foreign key" << slave_key << "to value of master key:" << row.value(master_key).toString();
						row.setValue(slave_key, row.value(master_key));
					}
				}
			}
		}
		else {
			qfDebug() << "\tEDIT";
			qf::core::sql::Connection sql_conn = sqlConnection();
			QSqlDriver *sqldrv = sql_conn.driver();
			for(QString table_id : tableIds(m_table.fields())) {
				qfDebug() << "\ttableid:" << table_id;
				//table = conn.fullTableNameToQtDriverTableName(table);
				QSqlRecord edit_rec;
				int i = -1;
				bool has_blob_field = true;
				for(qfu::Table::Field fld : row.fields()) {
					i++;
					qfDebug() << "\t\tfield:" << fld.toString();
					if(fld.tableId() != table_id)
						continue;
					if(!row.isDirty(i))
						continue;
					if(!fld.canUpdate()) {
						qfWarning() << "field" << fld.name() << "is dirty, but it has not canUpdate flag";
						continue;
					}
					QVariant v = row.value(i);
					//qfDebug() << "\ttableid:" << tableid << "fullTableName:" << fld.fullTableName();
					qfDebug() << "\tdirty field" << fld.name() << "type:" << fld.type() << "orig val:" << row.origValue(i).toString() << "new val:" << v.toString();
					//qfDebug().noSpace() << "\tdirty value: '" << v.toString() << "' isNull(): " << v.isNull() << " type(): " << v.type();
					QSqlField sqlfld(fld.shortName(), fld.type());
					sqlfld.setValue(v);
					//if(sqlfld.type() == QVariant::ByteArray)
					//	has_blob_field = true;
					qfDebug() << "\tfield is null: " << sqlfld.isNull();
					edit_rec.append(sqlfld);
				}
				if(!edit_rec.isEmpty()) {
					qfDebug() << "updating table edits:" << table_id;
					QString query_str;
					query_str += sqldrv->sqlStatement(QSqlDriver::UpdateStatement, table_id, edit_rec, has_blob_field);
					query_str += " ";
					QSqlRecord where_rec;
					for(QString fld_name : sql_conn.primaryIndexFieldNames(table_id)) {
						QString full_fld_name = table_id + '.' + fld_name;
						int fld_ix = m_table.fields().fieldIndex(full_fld_name);
						QF_ASSERT(fld_ix >= 0,
								  QString("Cannot find field '%1'").arg(full_fld_name),
								  continue);
						qfu::Table::Field fld = m_table.fields().at(fld_ix);
						QSqlField sqlfld(fld.shortName(), fld.type());
						sqlfld.setValue(row.origValue(fld_ix));
						//qfDebug() << "\tpri index" << f.name() << ":" << f.value().toString() << "value type:" << QVariant::typeToName(f.value().type()) << "field type:" << QVariant::typeToName(f.type());
						where_rec.append(sqlfld);
					}
					QF_ASSERT(!where_rec.isEmpty(),
							  QString("pri keys values not generated for table '%1'").arg(table_id),
							  continue);

					query_str += sqldrv->sqlStatement(QSqlDriver::WhereStatement, table_id, where_rec, false);
					qfDebug() << "save edit query:" << query_str;
					qfs::Query q(sql_conn);
					bool ok;
					if(has_blob_field) {
						q.prepare(query_str);
						for(int i=0; i<edit_rec.count(); i++)
							q.addBindValue(edit_rec.field(i).value());
						ok = q.exec();
					}
					else {
						ok = q.exec(query_str);
					}
					if(!ok && throw_exc) {
						QF_EXCEPTION(q.lastError().text());
					}
					qfDebug() << "\tnum rows affected:" << q.numRowsAffected();
					int num_rows_affected = q.numRowsAffected();
					/// if update command does not really change data for ex. (UPDATE woffice.kontakty SET id=8 WHERE id = 8)
					/// numRowsAffected() returns 0.
					if(num_rows_affected > 1) {
						qfError() << QString("numRowsAffected() = %1, sholuld be 1 or 0\n%2").arg(num_rows_affected).arg(query_str);
						ret = false;
						break;
					}
				}
			}
		}
		if(ret) {
			ret = Super::postRow(row_no, throw_exc);
		}
	}
	return ret;
}

bool SqlTableModel::removeTableRow(int row_no, bool throw_exc)
{
	qfLogFuncFrame();
	bool ret = false;
	qfu::TableRow row = m_table.row(row_no);
	if(!row.isInsert()) {
		qf::core::sql::Connection sql_conn = sqlConnection();
		qf::core::sql::Connection dbinfo(sql_conn);
		QSqlDriver *sqldrv = sql_conn.driver();

		if(row.isDirty())
			row.restoreOrigValues();
		QStringList table_ids = tableIdsSortedAccordingToForeignKeys();
		QSet<QString> referenced_foreign_tables = referencedForeignTables();
		int table_id_cnt = 0;
		for(const QString &table_id : table_ids) {
			/// Allways delete in first table
			if(table_id_cnt++ > 0) {
				/// delete in rest of the tables only if they are implicitly referenced, see: addForeignKeyDependency(...)
				if(!referenced_foreign_tables.contains(table_id)) {
					qfDebug() << "\tskipping table" << table_id;
					continue;
				}
			}

			QString table = dbinfo.fullTableNameToQtDriverTableName(table_id);
			QSqlRecord rec;
			qfDebug() << "deleting in table" << table;
			QString query_str;
			query_str += sqldrv->sqlStatement(QSqlDriver::DeleteStatement, table, rec, false);
			query_str += " ";
			QSqlRecord where_rec;
			for(QString fld_name : sql_conn.primaryIndexFieldNames(table_id)) {
				QString full_fld_name = table_id + '.' + fld_name;
				int fld_ix = m_table.fields().fieldIndex(full_fld_name);
				QF_ASSERT(fld_ix >= 0,
						  QString("Cannot find field '%1'").arg(full_fld_name),
						  continue);
				qfu::Table::Field fld = m_table.fields().at(fld_ix);
				QVariant id = row.value(fld_ix);
				QSqlField sqlfld(fld.shortName(), fld.type());
				sqlfld.setValue(id);
				bool invalid_id = false;
				if(id.isNull())
					invalid_id = true;
				else if(id.type() == QVariant::Int && id.toInt() == 0)
					invalid_id= true;
				if(!invalid_id) {
					sqlfld.setValue(id);
					qfDebug() << "\tpri index" << fld_name << ":" << sqlfld.value().toString();
					rec.append(sqlfld);
				}
				else {
					qfWarning() << "DELETE Invalid row ID:" << id.toString() << "type:" << id.typeName();
					break;
				}
				//qfDebug() << "\tpri index" << f.name() << ":" << f.value().toString() << "value type:" << QVariant::typeToName(f.value().type()) << "field type:" << QVariant::typeToName(f.type());
				where_rec.append(sqlfld);
			}
			QF_ASSERT(!where_rec.isEmpty(),
					  QString("pri keys values not generated for table '%1'").arg(table_id),
					  continue);

			query_str += sqldrv->sqlStatement(QSqlDriver::WhereStatement, table_id, where_rec, false);
			qfDebug() << "drop row query:" << query_str;
			qfs::Query q(sql_conn);
			ret = q.exec(query_str);
			if(!ret) {
				if(throw_exc) {
					QF_EXCEPTION(q.lastError().text());
				}
				else {
					qfError() << q.lastError().text();
					break;
				}
			}
			qfDebug() << "\tnum rows affected:" << q.numRowsAffected();
			int num_rows_affected = q.numRowsAffected();
			/// if update command does not really change data for ex. (UPDATE woffice.kontakty SET id=8 WHERE id = 8)
			/// numRowsAffected() returns 0.
			if(num_rows_affected != 1) {
				qfError() << QString("numRowsAffected() = %1, sholuld be 1\n%2").arg(num_rows_affected).arg(query_str);
				ret = false;
				break;
			}
		}
	}
	if(ret) {
		ret = Super::removeTableRow(row_no, throw_exc);
	}
	return ret;
}

void SqlTableModel::revertRow(int row_no)
{
	qfLogFuncFrame() << row_no;
}

int SqlTableModel::reloadRow(int row_no)
{
	qfLogFuncFrame() << "row:" << row_no << "row count:" << rowCount();
	qf::core::sql::QueryBuilder qb = m_queryBuilder;
	if(qb.isEmpty()) {
		qfWarning() << "Empty queryBuilder";
		return false;
	}
	qfu::TableRow &row_ref = m_table.rowRef(row_no);
	qf::core::sql::Connection sql_conn = sqlConnection();
	QSqlDriver *sqldrv = sql_conn.driver();
	for(QString table_id : tableIds(m_table.fields())) {
		qfDebug() << "\ttableid:" << table_id;
		for(QString fld_name : sql_conn.primaryIndexFieldNames(table_id)) {
			QString full_fld_name = table_id + '.' + fld_name;
			int fld_ix = m_table.fields().fieldIndex(full_fld_name);
			QF_ASSERT(fld_ix >= 0,
					  QString("Cannot find field '%1'").arg(full_fld_name),
					  continue);
			/*
				QVariant val = row.origValue(fld_ix);
				QString formated_val = val.toString();
				if(val.type() == QVariant::String) {
					formated_val.replace('\'', "''");
					formated_val = '\'' + formated_val + '\'';
				}
				*/
			qfu::Table::Field fld = m_table.fields().at(fld_ix);
			QSqlField sqlfld(fld.shortName(), fld.type());
			// cannot use origValue() here, since reloadRow() must work for even edited primary keys
			sqlfld.setValue(row_ref.value(fld_ix));
			QString formated_val = sqldrv->formatValue(sqlfld);
			qb.where(full_fld_name + "=" + formated_val);
		}
	}
	qfs::Query q = qfs::Query(sql_conn);
	QString query_str = qb.toString();
	query_str = replaceQueryParameters(query_str);
	qfDebug() << "\t reload row query:" << query_str;
	bool ok = q.exec(query_str);
	QF_ASSERT(ok == true,
			  QString("SQL Error: %1\n%2").arg(q.lastError().text()).arg(query_str),
			  return false);
	int row_cnt = 0;
	while(q.next()) {
		if(row_cnt++) {
			qfWarning() << "More than 1 row returned by query.\n" + query_str;
			continue;
		}
		for(int i=0; i<row_ref.fields().count(); i++) {
			row_ref.setBareBoneValue(i, q.value(i));
			qfDebug() << "\t\t" << i << "->" << row_ref.value(i).toString();
		}
	}
	Super::reloadRow(row_no);
	return row_cnt;
}

QString SqlTableModel::buildQuery()
{
	QString ret = m_query;
	if(ret.isEmpty()) {
		ret = m_queryBuilder.toString();
	}
	return ret;
}

static QString paramValueToString(const QVariant &v)
{
	QString ret;
	if(v.isValid())
		ret = v.toString();
	else
		ret = QStringLiteral("NULL");
	return ret;
}

QString SqlTableModel::replaceQueryParameters(const QString query_str)
{
	QString ret = query_str;
	QVariant par_v = queryParameters();
	if(par_v.type() == QVariant::Map) {
		QVariantMap par_map = par_v.toMap();
		QMapIterator<QString, QVariant> it(par_map);
		while(it.hasNext()) {
			it.next();
			QString key = "{{" + it.key() + "}}";
			QString val_str = paramValueToString(it.value());
			ret.replace(key, val_str, Qt::CaseInsensitive);
		}
	}
	else {
		QString val_str = paramValueToString(par_v);
		ret.replace("{{id}}", val_str, Qt::CaseInsensitive);
	}
	return ret;
}

qf::core::sql::Connection SqlTableModel::sqlConnection()
{
	QSqlDatabase db = QSqlDatabase::database(connectionName());
	qf::core::sql::Connection ret = qf::core::sql::Connection(db);
	QF_CHECK(ret.isValid(),
			 QString("Invalid sql connection for name '%1'").arg(connectionName()));
	return ret;
}

bool SqlTableModel::reloadTable(const QString &query_str)
{
	qf::core::sql::Connection sql_conn = sqlConnection();
	m_recentlyExecutedQuery = qfs::Query(sql_conn);
	bool ok = m_recentlyExecutedQuery.exec(query_str);
	QF_ASSERT(ok == true,
			  QString("SQL Error: %1\n%2").arg(m_recentlyExecutedQuery.lastError().text()).arg(query_str),
			  return false);
	if(m_recentlyExecutedQuery.isSelect()) {
		qfu::Table::FieldList table_fields;
		QSqlRecord rec = m_recentlyExecutedQuery.record();
		int fld_cnt = rec.count();
		for(int i=0; i<fld_cnt; i++) {
			QSqlField rec_fld = rec.field(i);
			qfu::Table::Field fld(rec_fld.name(), rec_fld.type());
			table_fields << fld;
		}
		setSqlFlags(table_fields, query_str);
		m_table = qfu::Table(table_fields);
		while(m_recentlyExecutedQuery.next()) {
			qfu::TableRow &row = m_table.appendRow();
			row.setInsert(false);
			for(int i=0; i<fld_cnt; i++) {
				row.setBareBoneValue(i, m_recentlyExecutedQuery.value(i));
			}
		}
	}
	emit reloaded();
	return true;
}

static QString compose_table_id(const QString &table_name, const QString &schema_name)
{
	QString ret = table_name;
	if(!schema_name.isEmpty())
		ret = schema_name + '.' + ret;
	return ret;
}

QSet<QString> SqlTableModel::tableIds(const qf::core::utils::Table::FieldList &table_fields)
{
	QSet<QString> ret;
	int fld_cnt = table_fields.count();
	for(int i=0; i<fld_cnt; i++) {
		const qfu::Table::Field &fld = table_fields[i];
		QString fs, ts, ds;
		qf::core::Utils::parseFieldName(fld.name(), &fs, &ts, &ds);
		ts = compose_table_id(ts, ds);
		if(!ts.isEmpty())
			ret << ts;
	}
	return ret;
}

void SqlTableModel::setSqlFlags(qf::core::utils::Table::FieldList &table_fields, const QString &query_str)
{
	qfLogFuncFrame();
	QSet<QString> table_ids = tableIds(table_fields);
	QSet<QString> field_ids;
	int fld_cnt = table_fields.count();
	for(const qfu::Table::Field &fld : table_fields) {
		QString fs;
		qf::core::Utils::parseFieldName(fld.name(), &fs);
		field_ids << fs;
	}
	if(table_ids.isEmpty()) {
		/// SQL driver doesn't support table names in returned QSqlRecord
		/// try to guess it from select
		int ix1 = query_str.indexOf(QLatin1String(" JOIN "), Qt::CaseInsensitive);
		if(ix1 < 0) {
			QLatin1String from(" FROM ");
			ix1 = query_str.indexOf(from, Qt::CaseInsensitive);
			if(ix1 > 0) {
				ix1 += from.size();
				int ix2 = query_str.indexOf(' ', ix1);
				if(ix2 < 0)
					ix2 = query_str.length();
				if(ix2 > ix1) {
					QString table_id_from_query = query_str.mid(ix1, ix2 - ix1);
					for(int i=0; i<fld_cnt; i++) {
						qfu::Table::Field &fld = table_fields[i];
						fld.setName(table_id_from_query + '.' + fld.name());
					}
					table_ids << table_id_from_query;
				}
			}
		}
	}
	QMap<QString, QString> serial_field_names;
	QSet<QString> updateable_table_ids;
	for(QString table_id : table_ids) {
		QString serial_field_name = sqlConnection().serialFieldName(table_id);
		qfDebug() << "serial field for table id:" << table_id << "is:" << serial_field_name;
		serial_field_names[table_id] = serial_field_name;
		QStringList prikey_fields = sqlConnection().primaryIndexFieldNames(table_id);
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
		qfu::Table::Field &fld = table_fields[i];
		QString table_id = fld.tableId();
		if(!table_id.isEmpty()) {
			fld.setCanUpdate(updateable_table_ids.contains(table_id));
			QStringList prikey_fields = sqlConnection().primaryIndexFieldNames(table_id);
			fld.setPriKey(prikey_fields.contains(fld.shortName()));
			//qfDebug() << "table_id;" << table_id << "fldname:" << fld.shortName() << "serial_field_names.value(table_id):" << serial_field_names.value(table_id);
			fld.setSerial(serial_field_names.value(table_id) == fld.shortName());
			qfDebug() << fld.name() << "is serial:" << fld.isSerial();
		}
	}
}

QSet<QString> SqlTableModel::referencedForeignTables()
{
	qfLogFuncFrame();
	QSet<QString> ret;
	{
		QStringList sl = m_foreignKeyDependencies.values();
		for(QString s : sl) {
			QString tbl_name;
			qf::core::Utils::parseFieldName(s, nullptr, &tbl_name);
			tbl_name = tbl_name.trimmed().toLower();
			if(!tbl_name.isEmpty()) {
				//qfDebug() << "\t referenced_foreign_tables <<" << tbl_name;
				ret << tbl_name;
			}
		}
	}
	//qfDebug() << "\t returning:" << QStringList(referenced_foreign_tables.toList()).join(", ");
	return ret;
}

QStringList SqlTableModel::tableIdsSortedAccordingToForeignKeys()
{
	qfLogFuncFrame();
	QStringList ret;
	QStringList table_ids(tableIds(m_table.fields()).toList());

	while(!table_ids.isEmpty()) {
		int table_ix;
		for(table_ix=0; table_ix<table_ids.count(); table_ix++) {
			QString table_id = table_ids[table_ix];
			bool dependency_satisfied = true;
			/// check if this table_id depends on some different table
			QMapIterator<QString, QString> it(m_foreignKeyDependencies);
			while(it.hasNext()) {
				it.next();
				QString master_key = it.key();
				QString slave_key = it.value();
				QString field, table, schema;
				qf::core::Utils::parseFieldName(slave_key, &field, &table, &schema);
				slave_key = qf::core::Utils::composeFieldName(table, schema);
				if(qf::core::Utils::fieldNameCmp(table_id, slave_key)) {
					dependency_satisfied = false;
					/// table_id is a slave, so check if master table is in return list already
					qf::core::Utils::parseFieldName(master_key, &field, &table, &schema);
					master_key = qf::core::Utils::composeFieldName(table, schema);
					for(auto included_table_id : ret) {
						if(qf::core::Utils::fieldNameCmp(included_table_id, master_key)) {
							ret << table_ids.takeFirst();
							dependency_satisfied = true;
							break;
						}
					}
				}
				if(dependency_satisfied)
					break;
			}
			if(dependency_satisfied)
				break;
		}
		if(table_ix < table_ids.count()) {
			ret << table_ids.takeAt(table_ix);
		}
		else {
			qfError() << "Cannot statisfy table dependencies";
			ret.clear();
			break;
		}
	}

	return ret;
}

