#include "sqlquerytablemodel.h"
#include "../core/assert.h"

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
	bool ret = false;

	qfu::TableRow row = m_table.row(row_no);
	if(row.isDirty()) {
		if(row.isInsert()) {
			qfDebug() << "\tINSERT";
#if 0
			QFStringSet referenced_foreign_tables = referencedForeignTables(row);
			int tbl_cnt = 0;
			QStringList table_ids = tableIdsSortedAccordingToForeignKeys(row);
			foreach(QString tableid, table_ids) {
				qfDebug() << "\ttable:" << tableid;
				QString table_name = tableid.section('.', -1).trimmed().toLower();
				qfDebug() << "\t\t table_name:" << table_name;
				if(tbl_cnt++ > 0) {
					/// 1. tabulku ukladej vzdy, ostatni pouze pokud jsou definovany podminky pro ulozeni jejich foreign klicu
					if(!referenced_foreign_tables.contains(table_name)) {
						if(!row.isForcedInsert()) {
							qfDebug() << "\tSKIPPING" << table_name;
							continue;
						}
					}
				}
				QFSqlTableInfo ti = conn.catalog().table(tableid);
				//qfDebug() << "\t\t table info:" << ti.toString();
				QFString table = ti.fullName();
				table = conn.fullTableNameToQtDriverTableName(table);
				QSqlDriver *drv = conn.driver();
				{
					QSqlRecord rec;
					int i = -1;
					int autoinc_ix = -1;
					//QSqlIndex pri_ix = ti.primaryIndex();
					bool has_blob_field = false;
					foreach(QFSqlField f, row.fields()) {
						i++;
						if(f.fullTableName() != tableid) continue;
						//qfError() << ti.field(f.fieldName()).toString();
						bool add_field = row.isDirty(i);
						if(row.isForcedInsert()) {
							if(ti.field(f.fieldName()).isAutoIncrement()) {
								/// ve forcedInsert neni treba ukladat autoinkrement, protoze pro ulozeni radku je nutne, aby mel nektery field dirty
								autoinc_ix = i;
							}
						}
						else {
							if(ti.field(f.fieldName()).isAutoIncrement()) {
								/// autoinc field vzdy pridej do insertu, mohlo by se stat, ze by nesel ulozit prazdnej radek, protoze by nebyl zadny field dirty
								add_field = true;
								autoinc_ix = i;
							}
							if(ti.field(f.fieldName()).isPriKey()) {
								/// prikey field vzdy pridej do insertu, bez nej by to neslo (pokud je autoincrement, uplatni se i predchozi podminka)
								add_field = true;
							}
						}
						if(!add_field) continue;
						qfDebug() << "\tdirty field:" << f.name();
						qfDebug() << "\tnullable:" << f.isNullable();
						if(f.type() == QVariant::ByteArray) {
							qfDebug() << "\t\tBLOB size:" << row.value(i).toByteArray().size();
							has_blob_field = true;
						}
						QVariant v = row.value(i);
						if(!v.isNull()) {
							/// null hodnotu nema smysl insertovat, pokud to neni nutne kvuli necemu jinemu
							/// naopak se pri insertu dokumentu z vice join tabulek muze stat, ze se vlozi to not-null fieldu null hodnota
							/// pokud je insert, join radek neexistuje, takze hodnoty jsou null a mysql v takovem pripade v resultsetu nastavi
							/// i not-null field na nullable
							//qfInfo() << "\t" << f.fieldName() << "value type:" << QVariant::typeToName(v.type());
							f.setValue(v);
							//qfInfo() << "\t\t" << "val type:" << QVariant::typeToName(f.value().type());
							f.setName(f.fieldName());
							rec.append(f);
						}
					}
					if(!rec.isEmpty()) {
						qfDebug() << "updating table inserts" << table;
						QString s;
						s = drv->sqlStatement(QSqlDriver::InsertStatement, table, rec, has_blob_field);
						QFSqlQuery q = QFSqlQuery(conn);
						if(has_blob_field) {
							q.prepare(s);
							for(int i=0; i<rec.count(); i++) {
								QVariant::Type type = rec.field(i).value().type();
								//qfInfo() << "\t" << rec.field(i).name() << "bound type:" << QVariant::typeToName(type);
								qfDebug() << "\t\t" << rec.field(i).name() << "bound type:" << QVariant::typeToName(type) << "value:" << rec.field(i).value().toString().mid(0, 100);
								q.addBindValue(rec.field(i).value());
							}
							qfDebug() << "\texecuting prepared query:" << s;
							q.exec();
						}
						else {
							qfDebug() << "\texecuting:" << s;
							q.exec(s);
						}
						qfDebug() << "\tnum rows affected:" << q.numRowsAffected();
						int num_rows_affected = q.numRowsAffected();
						//setNumRowsAffected(q.numRowsAffected());
						if(num_rows_affected != 1) QF_INTERNAL_ERROR(tr("numRowsAffected() = %1, should be 1\n%2").arg(num_rows_affected).arg(s));
						if(autoinc_ix >= 0) {
							QVariant v = Qf::retypeVariant(q.lastInsertId(), row.fields()[autoinc_ix].type());
							row.setValue(autoinc_ix, v);
							row.setDirty(autoinc_ix, false);
						}
					}
				}
				if(props.foreignKeyMap().isEmpty()) {
					if(!row.isForcedInsert()) break; /// krome specialnich pripadu insert v JOINed SELECTu ma smysl jen pro jednu tabulku, takze tu prvni v dotazu.
				}
				else {
					/// insertni i do slave tabulek
					QSqlIndex pri_ix = ti.primaryIndex();
					for(int i=0; i<pri_ix.count(); i++) {
						QFSqlField f = pri_ix.field(i);
						QString master_key = QFSql::composeFullName(f.name(), table_name).toLower();
						qfDebug() << "\t master_key:" << master_key;
						QString slave_key = props.foreignKeyMap().value(master_key);
						if(!slave_key.isEmpty()) {
							qfDebug() << "\tsetting value of foreign key" << slave_key << "to value of master key:" << QFSql::formatValue(row.value(master_key));
							row.setValue(slave_key, row.value(master_key));
						}
					}
				}
			}
#endif
		}
		else {
			qfDebug() << "\tEDIT";
			QSqlDatabase sqldb = QSqlDatabase::database(connectionName());
			QSqlDriver *sqldrv = sqldb.driver();
			for(QString tableid : tableIds(m_table.fields())) {
				qfDebug() << "\ttableid:" << tableid;
				//table = conn.fullTableNameToQtDriverTableName(table);
				{
					QSqlRecord edit_rec;
					int i = -1;
					bool has_blob_field = false;
					for(qfu::Table::Field fld : row.fields()) {
						i++;
						if(fld.name() != tableid)
							continue;
						if(!row.isDirty(i))
							continue;
						if(!fld.canUpdate()) {
							qfWarning() << "field" << fld.name() << "is dirty, but it has not canUpdate flag";
							continue;
						}
						QVariant v = row.value(i);
						//qfDebug() << "\ttableid:" << tableid << "fullTableName:" << fld.fullTableName();
						//qfDebug().noSpace() << "\tdirty field '" << fld.name() << "' type(): " << fld.type();
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
						qfDebug() << "\t" << s;
						QSqlQuery q(sqldb);
						if(has_blob_field) {
							q.prepare(s);
							for(int i=0; i<edit_rec.count(); i++)
								q.addBindValue(edit_rec.field(i).value());
							q.exec();
						}
						else {
							q.exec(s);
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
	setSqlFlags(table_fields, query_str);
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
	QString table_id_from_query;
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
				if(ix2 > 0) {
					table_id_from_query = query_str.mid(ix1, ix2 - ix1);
					table_ids << table_id_from_query;
				}
			}
		}
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
		qfu::Table::Field &fld = table_fields[i];
		QString fs, ts, ds;
		qf::core::Utils::parseFieldName(fld.name(), &fs, &ts, &ds);
		if(ts.isEmpty())
			ts = table_id_from_query;
		QString table_id = compose_table_id(ts, ds);
		if(!table_id.isEmpty()) {
			QString full_field_name = table_id + '.' + fs;
			if(full_field_name != fld.name()) {
				qfDebug() << "qualifying field name:" << fld.name() << "->" << full_field_name;
				fld.setName(full_field_name);
			}
			fld.setCanUpdate(updateable_table_ids.contains(table_id));
			QStringList prikey_fields = primaryIndex(table_id);
			fld.setPriKey(prikey_fields.contains(fs));
		}
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
