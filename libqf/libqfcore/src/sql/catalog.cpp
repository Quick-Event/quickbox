#include "catalog.h"

#include "query.h"

#include "../core/log.h"
#include "../core/utils.h"
#include "../core/string.h"
#include "../core/assert.h"

#include <QSqlQuery>
#include <QSqlDriver>
#include <QTextStream>
#include <QIODevice>

#define QFCATALOG_MYSQL_VERSION_MAJOR 5

using namespace qf::core::sql;

//=========================================
// FieldInfo
//=========================================
void FieldInfo::setName(const QString &n)
{
	QString f, t, d;
	qf::core::Utils::parseFieldName(n, &f, &t, &d);
	setShortName(f);
	setFullTableName(qf::core::Utils::composeFieldName(t, d));
}

void FieldInfo::setShortName(const QString &n)
{
	 Super::setName(n);
}

QString FieldInfo::toString(const QString& indent) const
{
	QString s;
	QTextStream ts(&s, QIODevice::WriteOnly);
	ts << indent << "name: " << name() << "\n";
	ts << indent << "full name: " << fullName() << "\n";
#if QT_VERSION_MAJOR >= 6
	ts << indent << "type: " << metaType().name() << "\n";
#else
	ts << indent << "type: " << QVariant::typeToName(type()) << "\n";
#endif
	ts << indent << "valid: " << isValid() << "\n";
	ts << indent << "length: " << length() << "\n";
	ts << indent << "precision: " << precision() << "\n";
	ts << indent << "default value: " << defaultValue().toString() << "\n";
	ts << indent << "required: " << requiredStatus() << "\n";
	ts << indent << "autovalue: " << isAutoValue() << "\n";
	ts << indent << "generated: " << isGenerated() << "\n";
	ts << indent << "nullable: " << isNullable() << "\n";
	ts << indent << "read only: " << isReadOnly() << "\n";
	ts << indent << "requiredStatus: " << requiredStatus() << "\n";

	ts << indent << "native type: " << nativeType() << "\n";
	ts << indent << "pri key: " << isPriKey() << "\n";
	ts << indent << "auto increment: " << isAutoIncrement() << "\n";
	ts << indent << "sequence name: " << seqName() << "\n";
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	ts << flush;
#else
	ts << Qt::flush;
#endif
	return s;
}

void FieldInfoList::load(const QSqlDatabase &connection, const QString table_id)
{
	qfLogFuncFrame() << "table_id:" << table_id;
	qfInfo() << "loading field info for table:" << table_id;
	QF_ASSERT(connection.isValid(),
			  "invalid connection",
			  return);
	Super::load(connection, table_id);

	qf::core::sql::Connection conn(connection);
	qf::core::sql::Query q(connection);
	q.setForwardOnly(true);
	//QString s = "SELECT * FROM %1 WHERE 1=2";
	//s = s.arg(fullName());
	//q.exec(s);
	//QSqlRecord r = q.record();
	QString full_table_name = table_id;
	QString table_name, schema_name;
	qf::core::Utils::parseFieldName(table_id, &table_name, &schema_name);
	if(schema_name.isEmpty())
		schema_name = conn.currentSchema();
	QStringList primary_keys;
	{
		QSqlIndex sql_ix = connection.primaryIndex(full_table_name);
		for(int i=0; i<sql_ix.count(); i++) {
			QString fld_name = sql_ix.fieldName(i);
			qf::core::Utils::parseFieldName(fld_name, &fld_name);
			primary_keys << fld_name;
		}
	}

	QString driver_name = connection.driverName();
	QSqlRecord r = connection.record(full_table_name);
	for(int i=0; i<r.count(); i++) {
		QString short_field_name;
		qf::core::Utils::parseFieldName(r.field(i).name(), &short_field_name);
		FieldInfo &fi = addEntry(short_field_name);
		fi = r.field(i);
		fi.setReadOnly(false);
		fi.setName(full_table_name + "." + short_field_name);
		fi.setAutoIncrement(fi.isAutoValue());
		if(driver_name.endsWith("PSQL")) {
			// fill seqname from default value in form: nextval('events_id_seq'::regclass)
			const QLatin1String pre("nextval('");
			const QLatin1String pos("'::regclass)");
			QString def_val = fi.defaultValue().toString();
			if(def_val.startsWith(pre, Qt::CaseInsensitive) && def_val.endsWith(pos, Qt::CaseInsensitive)) {
				QString seq_name = def_val.mid(pre.size());
				seq_name = seq_name.mid(0, seq_name.size() - pos.size());
				seq_name = table_name + '.' + seq_name;
				//qfInfo() << seq_name;
				fi.setSeqName(seq_name);
				fi.setAutoIncrement(true);
				qfDebug() << "\t\t name:" << fi.name() << "seq name:" << fi.seqName();
			}
			//qfInfo() << fi.toString();
			/*
			QSqlQuery q1(connection);
			q1.setForwardOnly(true);
			QString s = full_table_name;
			QString qs = QString("SELECT pg_get_serial_sequence('%1', '%2');").arg(s).arg(fi.shortName());
			qfInfo() << qs;
			if(q1.exec(qs) && q1.next()) {
				QString seq_name = q1.value(0).toString();
				if(!seq_name.isEmpty()) {
					qfInfo() << seq_name;
					fi.setSeqName(seq_name);
					fi.setAutoIncrement(true);
					qfDebug() << "\t\t name:" << fi.name() << "seq name:" << fi.seqName();
				}
			}
			*/
		}
		else if(driver_name.endsWith("MYSQL")) {
		}
		// fill prikey flag
		if(primary_keys.contains(fi.shortName()))
			fi.setPriKey(true);
		//qfInfo() << "#1 FieldInfo:" << fi.toString();
	}
	if(driver_name.endsWith("PSQL")) {
		QString s = "SELECT * FROM information_schema.columns"
					" WHERE table_name = '%1' AND table_schema = '%2'"
					" ORDER BY ordinal_position";
		s = s.arg(table_name, schema_name);
		q.exec(s);
		while(q.next()) {
			FieldInfo &fi = this->operator[](q.value("column_name").toString());
			//f.setTableName(d->tablename);
			//f.setSchema(d->schema);
			// fill seqname
			fi.setReadOnly(false);
			fi.setName(full_table_name + "." + q.value("column_name").toString());
			fi.setDefaultValue(q.value("column_default"));
			fi.setNullable(q.value("is_nullable").toString().toUpper() == "YES");
			fi.setNativeType(q.value("data_type").toString());
			//qfInfo() << "#2 FieldInfo:" << fi.toString();
			//qfTrash() << "\n" << catalog()->toString();
		}
	}
	else if(driver_name.endsWith("MYSQL")) {
		int ver = QFCATALOG_MYSQL_VERSION_MAJOR;
		qf::core::String s;
		if(ver <= 4) {
			s = QString("SHOW FULL columns FROM %2.%1");
		}
		else {
			s = QString("SELECT * FROM information_schema.columns"
				" WHERE table_name = '%1' AND table_schema = '%2'"
				" ORDER BY ordinal_position");
		}
		s = s.arg(table_name, schema_name);
		q.exec(s);
		while(q.next()) {
			if(ver <= 4) {
				FieldInfo &fi = this->operator[](q.value("field").toString());
				fi.setReadOnly(false);
				//fi.setFullName(fullName() + "." + q.value("field").toString());
				fi.setNullable(q.value("null").toString().toUpper() == "YES");
				qf::core::String s_type = q.value("type").toString().toLower();
				s = s_type;
				int ix = s.indexOf("(");
				if(ix > 0)
					s = s.mid(0, ix);
				fi.setNativeType(s);
				if(s == "enum" || s == "set") {
					int ix2 = s_type.indexOf(")");
					if(ix > 0 && ix2 > 0) {
						s = s_type.slice(ix+1, ix2);
						QStringList sl = s.splitAndTrim(',', '\'');
						fi.nativeValuesRef()["enumOrSetFields"] = sl;
					}
				}
				fi.setPriKey(q.value("key").toString().toUpper() == "PRI");
				fi.setAutoIncrement(q.value("extra").toString().toUpper() == "AUTO_INCREMENT");
				fi.setUnsigned(s_type.indexOf("unsigned") > 0);
				QVariant def_val;
				def_val = q.value("default");
				fi.setComment(q.value("comment").toString());
				/// pokud sloupec nemuze byt NULL, nemuze byt NULL ani jeho default value.
				/// S vyjjimkou AUTO_INCREMENT a TIMESTAMP
				if(def_val.isNull()) {
					if(!fi.isNullable() && !fi.isAutoIncrement()) def_val = QVariant("");
				}
				fi.setDefaultValue(def_val);
			}
			else {
				FieldInfo &fi = this->operator[](q.value("column_name").toString());
				fi.setReadOnly(false);
				//fi.setFullName(fullName() + "." + q.value("column_name").toString());
				fi.setNullable(q.value("is_nullable").toString().toUpper() == "YES");
				fi.setNativeType(q.value("data_type").toString());
				fi.setPriKey(q.value("column_key").toString().toUpper() == "PRI");
				fi.setAutoIncrement(q.value("extra").toString().toUpper() == "AUTO_INCREMENT");
				s = q.value("column_type").toString().toLower();
				fi.setUnsigned(s.indexOf("unsigned") > 0);
				if(fi.nativeType() == "enum" || fi.nativeType() == "set") {
					int ix = s.indexOf("(");
					int ix2 = s.indexOf(")");
					if(ix > 0 && ix2 > 0) {
						s = s.slice(ix+1, ix2);
						QStringList sl = s.splitAndTrim(',', '\'');
						fi.nativeValuesRef()["enumOrSetFields"] = sl;
					}
				}
				QVariant def_val;
				def_val = q.value("column_default");
				/// pokud sloupec nemuze byt NULL, nemuze byt NULL ani jeho default value.
				/// S vyjjimkou AUTO_INCREMENT a TIMESTAMP
				if(def_val.isNull()) {
					if(!fi.isNullable() && !fi.isAutoIncrement()) def_val = QVariant("");
				}
				fi.setDefaultValue(def_val);
				fi.setCharacterSet(q.value("character_set_name").toString());
				fi.setCollation(q.value("collation_name").toString());
				fi.setComment(q.value("column_comment").toString());
				if(q.value("data_type").toString() == "varchar") fi.setLength(q.value("character_maximum_length").toInt());
			}
		}
	}
	else if(driver_name.endsWith("SQLITE")) {
#if 0
		qf::core::String fs = connection.createTableSqlCommand(fullName());
		QStringList sl = SqlUtils::fieldDefsFromCreateTableCommand(fs);
		foreach(fs, sl) {
			// fill default values
			int ix = fs.indexOf("default", 0, Qt::CaseInsensitive);
			if(ix > 0) {
				QString nm = fs.section(' ', 0, 0);
				FieldInfo &f = fieldByName(ret, nm);
				if(!f.isValid()) {
					qfError() << QF_FUNC_NAME << tr("Found info for nonexisting field '%1'").arg(fs);
				}
				else {
					fs = fs.slice(ix);
					fs = fs.section(' ', 1, 1, QString::SectionSkipEmpty);
					if(!!fs) {
						if(fs[0] == '\'') fs = fs.slice(1, -1);
						//qfTrash() << "\tfound default value field:" << nm << "value:" << fs;
						f->setDefaultValue(fs);
					}
				}
			}
			// fill seqname
			ix = fs.indexOf("autoincrement", 0, Qt::CaseInsensitive);
			if(ix > 0) {
				QString nm = fs.section(' ', 0, 0);
				FieldInfo *f = fieldRef(nm, Qf::ThrowExc);
				if(!f) {
					qfError() << QF_FUNC_NAME << tr("Found info for nonexisting field '%1'").arg(fs);
				}
				else {
					f->setSeqName(tableName());
					f->setAutoIncrement(true);
				}
			}
		}
#endif
	}
}

void IndexInfoList::load(const QSqlDatabase &connection, const QString table_id)
{
	Super::load(connection, table_id);
	qf::core::sql::Connection::IndexList lst = qf::core::sql::Connection(connection).indexes(table_id);
	for(const auto& ii :lst) {
		addEntry(ii.name) = ii;
	}
}

