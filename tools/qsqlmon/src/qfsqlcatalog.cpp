#include "qfsqlcatalog.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>
#include <qf/core/string.h>
#include <qf/core/assert.h>

#include <QSqlQuery>
#include <QSqlDriver>

#define QFCATALOG_MYSQL_VERSION_MAJOR 5

QStringList SqlUtils::fieldDefsFromCreateTableCommand(const QString &cmd)
{
	qfLogFuncFrame() << cmd;
	QFString fs = cmd;
	QStringList sl;
	do {
		int ix;
		if((ix= fs.pos('(')) <0) break;
				//qfTrash() << "\tix:" << ix << "fs:" << s;
		fs = fs.slice(ix);
				//qfTrash() << "\tfields start:" << fs;
		if((ix= fs.indexOfMatchingBracket('(', ')', '\'')) <0) break;
				//qfTrash() << "\tlen:" << fs.len() << "ix:" << ix;
		fs = fs.slice(1, ix);
		//qfTrash() << "\tfields:" << fs;
		sl = fs.splitBracketed(',', '(', ')', '\'');
	} while(false);
	return sl;
}

static QString err_msg(QProcess::ProcessError err_no)
{
	switch(err_no) {
		case QProcess::FailedToStart:
			return "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
		case QProcess::Crashed:
			return "The process crashed some time after starting successfully.";
		case QProcess::Timedout:
			return "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
		case QProcess::WriteError:
			return "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
		case QProcess::ReadError:
			return "An error occurred when attempting to read from the process. For example, the process may not be running.";
		default:
			return "An unknown error occurred. This is the default return value of error().";
	}
}

QString SqlUtils::createTableSqlCommand(const QString &tblname)
{
	qfLogFuncFrame();
	if(driverName().endsWith("SQLITE")) {
		QString db, tbl;
		qf::core::Utils::parseFieldName(tblname, &tbl, &db);
		//db = normalizeDbName(db);
		QSqlQuery q(driver()->createResult());
		q.setForwardOnly(true);
		QString s = "SELECT sql FROM %1.sqlite_master"
				" WHERE type IN ('table', 'view')"
				" AND tbl_name = '%2'";
		s = s.arg(db).arg(tbl);
		//qfTrash() << '\t' << s;
		if(!q.exec(s)) {
			QSqlError err = lastError();
			qfError() << trUtf8("Error getting sql for '%1'").arg(tblname) + QF_EOLN + err.driverText() + QF_EOLN + err.databaseText();
				//QF_SQL_EXCEPTION(s);
		}
		else 	while(q.next()) {
			return q.value(0).toString() + ";";
		}
	}
	else if(driverName().endsWith("PSQL")) {
		QString db, tbl;
		qf::core::Utils::parseFieldName(tblname, &tbl, &db);
		QProcess proc;
		QString prog_name = "pg_dump";
		QStringList params;
		params << "-h";
		params << hostName();
		params << "-p";
		params << QString::number(port());
		params << "-U";
		params << userName();
		params << "-t";
		params << tbl;
		params << databaseName();
		QString s = prog_name + " " + params.join(" ");
		proc.start(prog_name, params);
		qfTrash() << "\tcalling:" << s;
		if (!proc.waitForStarted(5000)) {
			return "calling: " + s + "\n" + err_msg(proc.error());
		}
		//proc.write("Qt rocks!");
		//proc.closeWriteChannel();
		if (!proc.waitForFinished(10000)) {
			return "finished error: " + err_msg(proc.error());
		}
		s = QString();
		if(proc.exitCode() != 0) {
			s = "EXIT CODE: %1\n";
			s = s.arg(proc.exitCode());
			s += QString(proc.readAllStandardError());
			s += "\n";
		}
		QByteArray result = proc.readAll();
		return s + QString(result);
	}
	if(driverName().endsWith("MYSQL")) {
		QSqlQuery q(driver()->createResult());
		q.setForwardOnly(true);
		QString s = "SHOW CREATE TABLE %1";
		s = s.arg(tblname);
		qfTrash() << '\t' << s;
		if(!q.exec(s)) {
			QSqlError err = lastError();
			//qfTrash() << "\terr is valid:" << err.isValid();
			QString msg  = trUtf8("Error getting sql for '%1'").arg(tblname) + QF_EOLN + s + QF_EOLN + err.driverText() + QF_EOLN + err.databaseText();
			QF_SQL_EXCEPTION(msg);
		}
		else if(q.next()) {
			return q.value(1).toString() + ";";
		}
		return QString();
	}
	return "unsupported for " + driverName();
}

QString SqlUtils::dumpTableSqlCommand(const QString &tblname)
{
	if(driverName().endsWith("MYSQL")) {
		QSqlQuery q(driver()->createResult());
		q.setForwardOnly(true);
		QString s = "SELECT * FROM %1";
		s = s.arg(tblname);
		//qfTrash() << '\t' << s;
		if(!q.exec(s)) {
			QSqlError err = lastError();
			//qfTrash() << "\terr is valid:" << err.isValid();
			QString s  = trUtf8("Error select from '%1'").arg(tblname) + QF_EOLN + err.driverText() + QF_EOLN + err.databaseText();
			QF_SQL_EXCEPTION(s);
		}
		else {
			QStringList sl;
			while(q.next()) {
				QSqlRecord rec = q.record();
				QStringList sl2;
				for(int i=0; i<rec.count(); i++) {
					sl2 << driver()->formatValue(rec.field(i));
				}
				sl << "(" + sl2.join(",") + ")";
			}
			QString table;
			qf::core::Utils::parseFieldName(tblname, &table);
			s = "INSERT INTO %1 VALUES\n";
			s = s.arg(table) + sl.join(",\n");
			s += ";";
			return s;
		}
		return QString();
	}
	return "unsupported for " + driverName();
}

/*
static QFSqlFieldInfo& fieldByName(QList<QFSqlFieldInfo> &field_list, const QString &field_name)
{
	static QFSqlFieldInfo null_fi;
	for(int i=0; i<field_list.count(); i++) {
		QFSqlFieldInfo &fi = field_list[i];
		if(qf::core::Utils::fieldNameCmp(fi.fullName(), field_name)) {
			return fi;
		}
	}
	qfError() << "Cannot find file of name:" << field_name;
	return null_fi;
}
*/

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

QList<QFSqlIndexInfo> QFSqlIndexInfo::loadIndexes(const QSqlDatabase &connection, const QString table_id)
{
	qfLogFuncFrame() << "tblname:" << table_id;
	QString tblname = table_id;
	//if(tblname[0] == '.') tblname = tblname.mid(1);
	QList<QFSqlIndexInfo> ret;
	QString s;
	QString driver_name = connection.driver_name;
	if(driver_name.endsWith("PSQL")) {
		QString s = "SELECT indexname FROM pg_indexes WHERE tablename="QF_SARG(tbl_name);
		QSqlQuery q1(driver()->createResult());
		q1.setForwardOnly(true);
		q1.exec(s);
		while(q1.next()) {
			QString indexname = q1.value(0).toString();
			IndexInfo df;
			df.name = indexname;
			QString indexview = "SELECT indexes.relname AS indexname, indisprimary AS isprimary, indisunique AS isunique, columns.attname AS colname"
								" FROM pg_index LEFT JOIN pg_class AS indexes ON pg_index.indexrelid = indexes.oid"
								" LEFT JOIN pg_attribute AS columns ON columns.attrelid = pg_index.indrelid"
								" WHERE pg_index.indrelid='%1'::regclass AND columns.attnum = ANY (indkey)";
			indexview = indexview.arg(tblname);
			s  = "SELECT colname, isunique, isprimary FROM (%1) AS t WHERE indexname = '%2'";
			QSqlQuery q(driver()->createResult());
			q.setForwardOnly(true);
			q.exec(s.arg(indexview).arg(indexname));
			int i = 0;
			while(q.next()) {
				df.fields.append(q.value(0).toString());
				if(i++ == 0) df.unique = q.value(1).toBool();
			}
			ret.append(df);
		}
	}
	else if(driver_name.endsWith("MYSQL")) {
		QString s = "SHOW INDEX FROM " + tblname;
		qfTrash() << "\t" << s;
		QSqlQuery q(driver()->createResult());
		q.setForwardOnly(true);
		q.exec(s);
		IndexInfo df;
		while(q.next()) {
			s = q.value(2).toString();
			qfTrash() << "\tkey_name:" << s;
			if(df.name != s) {
				if(!df.name.isEmpty()) ret.append(df);
				df = IndexInfo();
				df.name = s;
				df.unique = !q.value(1).toBool();
				df.primary = (df.name == "PRIMARY");
				df.fields.append(q.value(4).toString());
			}
			else {
				df.fields.append(q.value(4).toString()); //q.record().indexOf("Column_name")
			}
		}
		if(!df.name.isEmpty()) ret.append(df);
	}
	else if(driver_name.endsWith("SQLITE")) {
		/// SQLITE neumi schema.tablename v prikazu PRAGMA table_info(...)
		QString tbl_name = tblname;
		{
			int ix = tbl_name.lastIndexOf('.');
			if(ix >= 0) tbl_name = tbl_name.mid(ix + 1);
		}
		QString s = "SELECT * FROM sqlite_master WHERE type='index' AND sql>'' AND tbl_name='%1'";
		s = s.arg(tbl_name);
		qfTrash() << "\t" << s;
		QSqlQuery q(driver()->createResult());
		q.setForwardOnly(true);
		q.exec(s);
		while(q.next()) {
			s = q.value(1).toString(); /// name
			qfTrash() << "\tkey_name:" << s;
			IndexInfo df;
			df.name = s;
			s = q.value(4).toString(); /// sql
			df.unique = s.indexOf(" UNIQUE ", Qt::CaseInsensitive) > 0;
			{
				int ix = s.lastIndexOf('(');
				if(ix > 0) {
					s = s.mid(ix+1);
					s = s.mid(0, s.count()-1);
					QStringList sl = s.split(',');
					df.fields = sl;
				}
			}
			if(!df.name.isEmpty()) ret.append(df);
		}
	}
	return ret;
}



