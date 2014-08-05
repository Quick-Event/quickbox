#include "sqlquerytablemodel.h"
#include "../core/assert.h"
#include "../core/exception.h"
#include "../sql/dbinfo.h"
#include "../sql/catalog.h"

#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlField>
#include <QSqlDriver>

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

void SqlQueryTableModel::addForeignKeyDependency(const QString &master_table_key, const QString &slave_table_key)
{
	m_foreignKeyDependencies[master_table_key] = slave_table_key;
}

bool SqlQueryTableModel::reload()
{
	QString qs = buildQuery();
	qs = replaceQueryParameters(qs);
	return reload(qs);
}

bool SqlQueryTableModel::reload(const QString &query_str)
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

bool SqlQueryTableModel::postRow(int row_no, bool throw_exc)
{
	qfLogFuncFrame() << row_no;

	qfu::TableRow row = m_table.row(row_no);
	bool ret = true;
	if(row.isDirty()) {
		if(row.isInsert()) {
			qfDebug() << "\tINSERT";

			//QSet<QString> referenced_foreign_tables = referencedForeignTables();
			//int tbl_cnt = 0;
			QSqlDatabase sqldb = QSqlDatabase::database(connectionName());
			qf::core::sql::DbInfo dbinfo(sqldb);
			QSqlDriver *sqldrv = sqldb.driver();
			QStringList table_ids = tableIdsSortedAccordingToForeignKeys();
			for(QString table_id : table_ids) {
				qfDebug() << "\ttable:" << table_id;
				//QFSqlTableInfo ti = conn.catalog().table(table_id);
				//qfDebug() << "\t\t table info:" << ti.toString();
				//QFString table = ti.fullName();
				//table = conn.fullTableNameToQtDriverTableName(table);
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
						/// always include serial fields, an empty lina cannot be inserted in other case
						is_field_dirty = true;
						serial_ix = i;
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
					if(!v.isNull()) {
						/// null hodnotu nema smysl insertovat, pokud to neni nutne kvuli necemu jinemu
						/// naopak se pri insertu dokumentu z vice join tabulek muze stat, ze se vlozi to not-null fieldu null hodnota
						/// pokud je insert, join radek neexistuje, takze hodnoty jsou null a mysql v takovem pripade v resultsetu nastavi
						/// i not-null field na nullable
						//qfInfo() << "\t" << f.fieldName() << "value type:" << QVariant::typeToName(v.type());
						QSqlField new_fld;
						new_fld.setValue(v);
						//qfInfo() << "\t\t" << "val type:" << QVariant::typeToName(f.value().type());
						new_fld.setName(fld.shortName());
						rec.append(new_fld);
					}
				}
				if(!rec.isEmpty()) {
					qfDebug() << "updating table inserts" << table_id;
					QString table = dbinfo.fullTableNameToQtDriverTableName(table_id);
					QString s = sqldrv->sqlStatement(QSqlDriver::InsertStatement, table, rec, true);
					QSqlQuery q(sqldb);
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
			QSqlDatabase sqldb = QSqlDatabase::database(connectionName());
			QSqlDriver *sqldrv = sqldb.driver();
			for(QString tableid : tableIds(m_table.fields())) {
				qfDebug() << "\ttableid:" << tableid;
				//table = conn.fullTableNameToQtDriverTableName(table);
				QSqlRecord edit_rec;
				int i = -1;
				bool has_blob_field = false;
				for(qfu::Table::Field fld : row.fields()) {
					i++;
					qfDebug() << "\t\tfield:" << fld.toString();
					if(fld.tableId() != tableid)
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
					if(sqlfld.type() == QVariant::ByteArray)
						has_blob_field = true;
					qfDebug() << "\tfield is null: " << sqlfld.isNull();
					edit_rec.append(sqlfld);
				}
				if(!edit_rec.isEmpty()) {
					qfDebug() << "updating table edits:" << tableid;
					QString s;
					s += sqldrv->sqlStatement(QSqlDriver::UpdateStatement, tableid, edit_rec, has_blob_field);
					s += " ";
					QSqlRecord where_rec;
					for(QString fld_name : primaryIndex(tableid)) {
						QString full_fld_name = tableid + '.' + fld_name;
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
							  QString("pri keys values not generated for table '%1'").arg(tableid),
							  continue);

					s += sqldrv->sqlStatement(QSqlDriver::WhereStatement, tableid, where_rec, false);
					qfDebug() << "save edit query:" << s;
					QSqlQuery q(sqldb);
					bool ok;
					if(has_blob_field) {
						q.prepare(s);
						for(int i=0; i<edit_rec.count(); i++)
							q.addBindValue(edit_rec.field(i).value());
						ok = q.exec();
					}
					else {
						ok = q.exec(s);
					}
					if(!ok && throw_exc) {
						QF_EXCEPTION(q.lastError().text());
					}
					qfDebug() << "\tnum rows affected:" << q.numRowsAffected();
					int num_rows_affected = q.numRowsAffected();
					/// if update command does not really change data for ex. (UPDATE woffice.kontakty SET id=8 WHERE id = 8)
					/// numRowsAffected() returns 0.
					if(num_rows_affected > 1) {
						qfError() << QString("numRowsAffected() = %1, sholuld be 1 or 0\n%2").arg(num_rows_affected).arg(s);
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

bool SqlQueryTableModel::reloadTable(const QString &query_str)
{
	QSqlDatabase db = QSqlDatabase::database(connectionName());
	m_recentlyExecutedQuery = QSqlQuery(db);
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
	return true;
}

static QString compose_table_id(const QString &table_name, const QString &schema_name)
{
	QString ret = table_name;
	if(!schema_name.isEmpty())
		ret = schema_name + '.' + ret;
	return ret;
}

QSet<QString> SqlQueryTableModel::tableIds(const qf::core::utils::Table::FieldList &table_fields)
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

void SqlQueryTableModel::setSqlFlags(qf::core::utils::Table::FieldList &table_fields, const QString &query_str)
{
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
		serial_field_names[table_id] = serialFieldName(table_id);
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
		qfu::Table::Field &fld = table_fields[i];
		QString table_id = fld.tableId();
		if(!table_id.isEmpty()) {
			fld.setCanUpdate(updateable_table_ids.contains(table_id));
			QStringList prikey_fields = primaryIndex(table_id);
			fld.setPriKey(prikey_fields.contains(fld.shortName()));
			fld.setSerial(!serial_field_names.value(fld.shortName()).isEmpty());
		}
	}
}

QStringList SqlQueryTableModel::primaryIndex(const QString &table_id)
{
	static QMap<QString, QStringList> s_primaryIndexCache;
	QString pk_key = connectionName() + '.' + table_id;
	QStringList ret;
	if(!s_primaryIndexCache.contains(pk_key)) {
		QSqlDatabase db = QSqlDatabase::database(connectionName());
		QSqlIndex sql_ix = db.primaryIndex(table_id);
		for(int i=0; i<sql_ix.count(); i++) {
			QString fld_name = sql_ix.fieldName(i);
			qf::core::Utils::parseFieldName(fld_name, &fld_name);
			ret << fld_name;
		}
		s_primaryIndexCache[pk_key] = ret;
	}
	else {
		ret = s_primaryIndexCache.value(pk_key);
	}
	return ret;
}

QString SqlQueryTableModel::serialFieldName(const QString &table_id)
{
	static QMap<QString, QString> s_serialFieldNamesCache;
	QString pk_key = connectionName() + '.' + table_id;
	QString ret;
	if(!s_serialFieldNamesCache.contains(pk_key)) {
		QSqlDatabase db = QSqlDatabase::database(connectionName());
		qf::core::sql::FieldInfoList fldlst;
		fldlst.load(db, table_id);
		QMapIterator<QString, qf::core::sql::FieldInfo> it(fldlst);
		while(it.hasNext()) {
			it.next();
			qf::core::sql::FieldInfo fi = it.value();
			if(fi.isAutoIncrement()) {
				ret = it.key();
				break;
			}
		}
		s_serialFieldNamesCache[pk_key] = ret;
	}
	else {
		ret = s_serialFieldNamesCache.value(pk_key);
	}
	return ret;
}

QSet<QString> SqlQueryTableModel::referencedForeignTables()
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

QStringList SqlQueryTableModel::tableIdsSortedAccordingToForeignKeys()
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
