#include "qfsqlfieldinfo.h"

#include <qf/core/log.h>

#include <QSqlQuery>
#include <QSqlDriver>

QList<QFSqlFieldInfo> QFSqlFieldInfo::loadFields(const QSqlDatabase &connection, const QString table_id)
{
	qfLogFuncFrame();// << "reload:" << reload;
	QList<QFSqlFieldInfo> ret;
	if(!connection.isValid())
		return ret;
	if(true) {
		QFSqlQuery q(connection);
		q.setForwardOnly(true);
		//QString s = "SELECT * FROM %1 WHERE 1=2";
		//s = s.arg(fullName());
		//q.exec(s);
		//QSqlRecord r = q.record();
		QFString full_table_name = fullName();
		QSqlRecord r = connection.record(full_table_name);
		for(int i=0; i<r.count(); i++) {
			QFSqlFieldInfo f(QFSqlField(r.field(i)), catalog());
			f.setReadOnly(false);
			f.setFullName(fullName() + "." + f.fieldName());
			qfTrash() << "\t\tfound driver reported name:" << f.driverReportedName() << "isValid():" << f.isValid() << "type:" << f.type();
			qfTrash() << "\t\tfield:" << f.toString();
			if(connection.driverName().endsWith("PSQL")) {
				// fill seqname
				QFSqlQuery q1(connection);
				q1.setForwardOnly(true);
				QFString s = fullName();
				//if(s[0] == '.') s = s.slice(1);
				q1.exec(QString("SELECT pg_get_serial_sequence('%1', '%2');").arg(s).arg(f.fieldName()));
					//qfError() << QF_FUNC_NAME << QString("Error getting the sequence information for: '%1.%2'").arg(fullName()).arg(f.fieldName());
				while(q1.next()) {
					f.setSeqName(q1.value(0).toString());
					f.setAutoIncrement(true);
					qfTrash() << "\t\tseq name:" << f.seqName();
					break;
				}
				// fill prikey flag
				if(d->prikeys.contains(f.fieldName())) f.setPriKey(true);
				//d->unorderedFieldNames.append(f.name());
			}
			else if(connection.driverName().endsWith("MYSQL")) {
				// fill prikey flag
				if(d->prikeys.contains(f.fieldName())) f.setPriKey(true);
			}
			d->fields[f.fieldName().toLower()] = f;
		}
		if(connection.driverName().endsWith("PSQL")) {
			QString s = "SELECT * FROM information_schema.columns"
					" WHERE table_name = '%1' AND table_schema = '%2'"
					" ORDER BY ordinal_position";
			s = s.arg(tableName(), dbName());
			//qfTrash() << "\t\t###################################";
			//qfTrash() << "\t\t" << s;
			q.exec(s);
			while(q.next()) {
				QFSqlFieldInfo &f = *fieldRef(q.value("column_name").toString(), Qf::ThrowExc, true);
				//f.setTableName(d->tablename);
				//f.setSchema(d->schema);
				// fill seqname
				f.setReadOnly(false);
				f.setFullName(fullName() + "." + q.value("column_name").toString());
				f.setDefaultValue(q.value("column_default"));
				f.setNullable(q.value("is_nullable").toString().toUpper() == "YES");
				f.setNativeType(q.value("data_type").toString());
				//qfTrash() << "\n" << catalog()->toString();
			}
		}
		if(connection.driverName().endsWith("MYSQL")) {
			int ver = catalog()->serverVersionMajor();
			QFString s;
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
					QFSqlFieldInfo &f = *fieldRef(q.value("field").toString(), Qf::ThrowExc, true);
					f.setReadOnly(false);
					f.setFullName(fullName() + "." + q.value("field").toString());
					f.setNullable(q.value("null").toString().toUpper() == "YES");
					QFString s_type = q.value("type").toString().toLower();
					s = s_type;
					int ix = s.indexOf("(");
					if(ix > 0) s = s.slice(0, ix);
					f.setNativeType(s);
					if(s == "enum" || s == "set") {
						int ix2 = s_type.indexOf(")");
						if(ix > 0 && ix2 > 0) {
							s = s_type.slice(ix+1, ix2);
							QStringList sl = s.splitAndTrim(',', '\'');
							f.nativeValuesRef()["enumOrSetFields"] = sl;
						}
					}
					f.setPriKey(q.value("key").toString().toUpper() == "PRI");
					f.setAutoIncrement(q.value("extra").toString().toUpper() == "AUTO_INCREMENT");
					f.setUnsigned(s_type.indexOf("unsigned") > 0);
					QVariant def_val;
					def_val = q.value("default");
					f.setComment(q.value("comment").toString());
           			         /// pokud sloupec nemuze byt NULL, nemuze byt NULL ani jeho default value.
          			          /// S vyjjimkou AUTO_INCREMENT a TIMESTAMP
					if(def_val.isNull()) {
						if(!f.isNullable() && !f.isAutoIncrement()) def_val = QVariant("");
					}
					f.setDefaultValue(def_val);
				}
				else {
					QFSqlFieldInfo &f = *fieldRef(q.value("column_name").toString(), Qf::ThrowExc, true);
					f.setReadOnly(false);
					f.setFullName(fullName() + "." + q.value("column_name").toString());
					f.setNullable(q.value("is_nullable").toString().toUpper() == "YES");
					f.setNativeType(q.value("data_type").toString());
					f.setPriKey(q.value("column_key").toString().toUpper() == "PRI");
					f.setAutoIncrement(q.value("extra").toString().toUpper() == "AUTO_INCREMENT");
					s = q.value("column_type").toString().toLower();
					f.setUnsigned(s.indexOf("unsigned") > 0);
					if(f.nativeType() == "enum" || f.nativeType() == "set") {
						int ix = s.indexOf("(");
						int ix2 = s.indexOf(")");
						if(ix > 0 && ix2 > 0) {
							s = s.slice(ix+1, ix2);
							QStringList sl = s.splitAndTrim(',', '\'');
							f.nativeValuesRef()["enumOrSetFields"] = sl;
						}
					}
					QVariant def_val;
					def_val = q.value("column_default");
     			               /// pokud sloupec nemuze byt NULL, nemuze byt NULL ani jeho default value.
  			                  /// S vyjjimkou AUTO_INCREMENT a TIMESTAMP
					if(def_val.isNull()) {
						if(!f.isNullable() && !f.isAutoIncrement()) def_val = QVariant("");
					}
					f.setDefaultValue(def_val);
					f.setCharacterSet(q.value("character_set_name").toString());
					f.setCollation(q.value("collation_name").toString());
					f.setComment(q.value("column_comment").toString());
					if(q.value("data_type").toString() == "varchar") f.setLength(q.value("character_maximum_length").toInt());
				}
			}
		}
		else if(connection.driverName().endsWith("SQLITE")) {
			QFString fs = connection.createTableSqlCommand(fullName());
			QStringList sl = QFSqlConnectionBase::fieldDefsFromCreateTableCommand(fs);
			foreach(fs, sl) {
				// fill default values
				int ix = fs.indexOf("default", 0, Qt::CaseInsensitive);
				if(ix > 0) {
					QString nm = fs.section(' ', 0, 0);
					QFSqlFieldInfo *f = fieldRef(nm, Qf::ThrowExc, true);
					if(!f) {
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
		}
		d->indexes = connection.indexes(fullName());
		d->fieldsUpToDate = true;
		qfTrash() << "\t***TABLE INFO FIELDS loaded\n" << toString();
	}
	return d->unorderedFieldNames;
}

//=========================================
//                          QFSqlFieldInfo
//=========================================
#if 0
QVariant QFSqlFieldInfo::seqNextVal() throw(QFSqlException)
{
	QVariant ret;
	if(!isValid()) return ret;
	QFSqlConnectionBase conn = catalog()->connection();
	if(connection.driverName().endsWith("PSQL")) {
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
	else if(connection.driverName().endsWith("SQLITE")) {
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
	QString ind = indent;
	QString s;
	QTextStream ts(&s, QIODevice::WriteOnly);
	ts << ind << "nullable: " << isNullable() << QF_EOLN;
	ts << ind << "native type: " << nativeType() << QF_EOLN;
	ts << ind << "pri key: " << isPriKey() << QF_EOLN;
	ts << ind << "auto increment: " << isAutoIncrement() << QF_EOLN;
	ts << ind << "sequence name: " << seqName() << QF_EOLN;
	ts << flush;
	s = QFSqlField::toString(ind) + s;
	return s;
}

QList<QFSqlIndexInfo> QFSqlIndexInfo::loadIndexes(const QSqlDatabase &connection, const QString table_id)
{
	qfLogFuncFrame() << "tblname:" << table_id;
	QString tblname = table_id;
	//if(tblname[0] == '.') tblname = tblname.mid(1);
	QList<QFSqlIndexInfo> ret;
	QString s;
	QString driver_name =
	if(driverName().endsWith("PSQL")) {
		QString s = "SELECT indexname FROM pg_indexes WHERE tablename="SARG(tbl_name);
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
	else if(driverName().endsWith("MYSQL")) {
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
	else if(driverName().endsWith("SQLITE")) {
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
