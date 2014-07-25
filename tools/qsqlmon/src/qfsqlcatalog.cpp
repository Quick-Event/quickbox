#include "qfsqlcatalog.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>
#include <qf/core/string.h>
#include <qf/core/assert.h>

#include <QSqlQuery>
#include <QSqlDriver>

#define QFCATALOG_MYSQL_VERSION_MAJOR 5

//=========================================
//                          QFSqlFieldInfo
//=========================================
#if 0
QVariant QFSqlFieldInfo::seqNextVal() throw(QFSqlException)
{
	QVariant ret;
	if(!isValid()) return ret;
	SqlUtils conn = catalog()->connection();
	if(connection.driver_name.endsWith("PSQL")) {
		QString s = seqName();
		if(!s.isEmpty()) {
			QSqlQuery q(connection.driver()->createResult());
			q.setForwardOnly(true);
			if(!q.exec(QString("SELECT nextval('%1');").arg(s))) {
				QF_SQL_EXCEPTION(QObject::tr("Error getting the sequence nextval('%1')").arg(s));
			}
			while(q.next()) {ret = q.value(0); break;}
		}
	}
	else if(connection.driver_name.endsWith("SQLITE")) {
		QString s = seqName();
		if(!s.isEmpty()) {
			qfLogFuncFrame();
			QString t = dbName() + ".sqlite_sequence";
			QSqlQuery q(connection.driver()->createResult());
			q.setForwardOnly(true);
			bool ok = false;
			do {
				ok = q.exec(QString("SELECT seq FROM %1 WHERE name='%2'").arg(t).arg(s));
				if(!ok || !q.next()) break;
				int i = q.value(0).toInt() + 1;
				qfTrash() << "\t" << QString("UPDATE %1 SET seq = seq + 1 WHERE name='%2'").arg(t).arg(s);
				ok = q.exec(QString("UPDATE %1 SET seq = seq + 1 WHERE name='%2'").arg(t).arg(s));
				if(!ok) break;
				ok = q.exec(QString("SELECT seq FROM %1 WHERE name='%2'").arg(t).arg(s));
				if(!ok || !q.next()) break;
				int j = q.value(0).toInt();
				if(i != j) {ok = false; break;}
				qfTrash() << "\t" << "next val:" << j;
				ret = j;
				ok = true;
			} while(false);
			if(!ok) QF_SQL_EXCEPTION(QObject::tr("Error getting the sequence nextval('%1')").arg(s));
		}
	}
	return ret;
}
#endif
QString QFSqlFieldInfo::toString(const QString& indent) const
{
	QString s;
	QTextStream ts(&s, QIODevice::WriteOnly);
	ts << indent << "name: " << name() << "\n";
	ts << indent << "full name: " << fullName() << "\n";
	ts << indent << "type: " << QVariant::typeToName(type()) << "\n";
	ts << indent << "valid: " << isValid() << "\n";
	ts << indent << "length: " << length() << "\n";
	ts << indent << "precision: " << precision() << "\n";
	ts << indent << "default value: " << defaultValue().toString() << "\n";
	ts << indent << "required: " << requiredStatus() << "\n";
	ts << indent << "autovalue: " << isAutoValue() << "\n";
	ts << indent << "nulable: " << isNullable() << "\n";
	ts << indent << "read only: " << isReadOnly() << "\n";
	ts << indent << "requiredStatus: " << requiredStatus() << "\n";

	ts << indent << "nullable: " << isNullable() << "\n";
	ts << indent << "native type: " << nativeType() << "\n";
	ts << indent << "pri key: " << isPriKey() << "\n";
	ts << indent << "auto increment: " << isAutoIncrement() << "\n";
	ts << indent << "sequence name: " << seqName() << "\n";
	ts << flush;
	return s;
}

void QFSqlFieldInfoList::load(const QSqlDatabase &connection, const QString table_id)
{
	qfLogFuncFrame();// << "reload:" << reload;
	QF_ASSERT(connection.isValid(),
			  "invalid connection",
			  return);
	Super::load(connection, table_id);

	QSqlQuery q(connection);
	q.setForwardOnly(true);
	//QString s = "SELECT * FROM %1 WHERE 1=2";
	//s = s.arg(fullName());
	//q.exec(s);
	//QSqlRecord r = q.record();
	QString full_table_name = table_id;

	QStringList primary_keys;
	{
		QSqlIndex sql_ix = connection.primaryIndex(full_table_name);
		for(int i=0; i<sql_ix.count(); i++) {
			QString fld_name = sql_ix.fieldName(i);
			qf::core::Utils::parseFieldName(fld_name, &fld_name);
			primary_keys << fld_name;
		}
	}

	QSqlRecord r = connection.record(full_table_name);
	for(int i=0; i<r.count(); i++) {
		QString short_field_name;
		qf::core::Utils::parseFieldName(r.field(i).fieldName(), &short_field_name);
		QFSqlFieldInfo &fi = addEntry(short_field_name);
		fi = r.field(i);
		fi.setReadOnly(false);
		fi.setFullName(full_table_name + "." + short_field_name);
		//qfTrash() << "\t\tfound driver reported name:" << f.driverReportedName() << "isValid():" << f.isValid() << "type:" << f.type();
		//qfTrash() << "\t\tfield:" << f.toString();
		if(connection.driver_name.endsWith("PSQL")) {
			// fill seqname
			QSqlQuery q1(connection);
			q1.setForwardOnly(true);
			QString s = full_table_name;
			//if(s[0] == '.') s = s.slice(1);
			q1.exec(QString("SELECT pg_get_serial_sequence('%1', '%2');").arg(s).arg(fi.fieldName()));
			//qfError() << QF_FUNC_NAME << QString("Error getting the sequence information for: '%1.%2'").arg(fullName()).arg(f.fieldName());
			while(q1.next()) {
				fi.setSeqName(q1.value(0).toString());
				fi.setAutoIncrement(true);
				//qfTrash() << "\t\tseq name:" << f.seqName();
				break;
			}
			// fill prikey flag
			if(primary_keys.contains(fi.fieldName()))
				fi.setPriKey(true);
			//d->unorderedFieldNames.append(f.name());
		}
		else if(connection.driver_name.endsWith("MYSQL")) {
			// fill prikey flag
			if(primary_keys.contains(fi.fieldName()))
				fi.setPriKey(true);
		}
	}
	if(connection.driver_name.endsWith("PSQL")) {
		QString s = "SELECT * FROM information_schema.columns"
					" WHERE table_name = '%1' AND table_schema = '%2'"
					" ORDER BY ordinal_position";
		s = s.arg(tableName(), dbName());
		q.exec(s);
		while(q.next()) {
			QFSqlFieldInfo &fi = this->operator[](q.value("column_name").toString());
			//f.setTableName(d->tablename);
			//f.setSchema(d->schema);
			// fill seqname
			fi.setReadOnly(false);
			fi.setFullName(fullName() + "." + q.value("column_name").toString());
			fi.setDefaultValue(q.value("column_default"));
			fi.setNullable(q.value("is_nullable").toString().toUpper() == "YES");
			fi.setNativeType(q.value("data_type").toString());
			//qfTrash() << "\n" << catalog()->toString();
		}
	}
	if(connection.driver_name.endsWith("MYSQL")) {
		int ver = QFCATALOG_MYSQL_VERSION_MAJOR;
		QString s;
		if(ver <= 4) {
			s = "SHOW FULL columns FROM %2.%1";
		}
		else {
			s = "SELECT * FROM information_schema.columns"
				" WHERE table_name = '%1' AND table_schema = '%2'"
				" ORDER BY ordinal_position";
		}
		s = s.arg(tableName(), dbName());
		q.exec(s);
		while(q.next()) {
			if(ver <= 4) {
				QFSqlFieldInfo &fi = this->operator[](q.value("field").toString());
				fi.setReadOnly(false);
				//fi.setFullName(fullName() + "." + q.value("field").toString());
				fi.setNullable(q.value("null").toString().toUpper() == "YES");
				QFString s_type = q.value("type").toString().toLower();
				s = s_type;
				int ix = s.indexOf("(");
				if(ix > 0) s = s.slice(0, ix);
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
				QFSqlFieldInfo &fi = this->operator[](q.value("column_name").toString());
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
	else if(connection.driver_name.endsWith("SQLITE")) {
#if 0
		qf::core::String fs = connection.createTableSqlCommand(fullName());
		QStringList sl = SqlUtils::fieldDefsFromCreateTableCommand(fs);
		foreach(fs, sl) {
			// fill default values
			int ix = fs.indexOf("default", 0, Qt::CaseInsensitive);
			if(ix > 0) {
				QString nm = fs.section(' ', 0, 0);
				QFSqlFieldInfo &f = fieldByName(ret, nm);
				if(!f.isValid()) {
					qfError() << QF_FUNC_NAME << trUtf8("Found info for nonexisting field '%1'").arg(fs);
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
				QFSqlFieldInfo *f = fieldRef(nm, Qf::ThrowExc);
				if(!f) {
					qfError() << QF_FUNC_NAME << trUtf8("Found info for nonexisting field '%1'").arg(fs);
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

void QFSqlIndexInfoList::load(const QSqlDatabase &connection, const QString table_id)
{
	Super::load(connection, table_id);
	qf::core::sql::DbInfo::IndexList lst = qf::core::sql::DbInfo(connection).indexes(table_id);
	for(const auto& ii :lst) {
		addEntry(ii.name) = ii;
	}
}
