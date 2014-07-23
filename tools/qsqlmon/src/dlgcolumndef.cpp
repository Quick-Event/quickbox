//
// C++ Implementation: dlgcolumndef
//
// Description:
//
//
// Author: Fanda Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mainwindow.h"
#include "theapp.h"

#include "dlgcolumndef.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

//#include <qfmsgshowagain.h>
//#include <qfsqlquery.h>

namespace qfc = qf::core;

QMap< QString, QMap<QString, QStringList> > DlgColumnDef::f_collationsCache;

DlgColumnDef::DlgColumnDef(QWidget * parent, const QString& table)
	:QDialog(parent)
{
	qfc::Utils::parseFieldName(table, &tableName, &dbName);
	setupUi(this);
	QStringList types;
	if(connection().driverName().endsWith("SQLITE")) {
		types << "INTEGER" << "REAL" << "TEXT" << "BLOB";
	}
	else if(connection().driverName().endsWith("PSQL")) {
		types
				<< "smallint" //	2 bytes	small-range integer	-32768 to +32767
				<< "integer" //	4 bytes	usual choice for integer	-2147483648 to +2147483647
				<< "bigint" //	8 bytes	large-range integer	-9223372036854775808 to 9223372036854775807
				<< "decimal" //	variable	user-specified precision, exact	no limit
				<< "numeric" //	variable	user-specified precision, exact	no limit
				<< "real" //	4 bytes	variable-precision, inexact	6 decimal digits precision
				<< "double precision" // precision	8 bytes	variable-precision, inexact	15 decimal digits precision
				<< "serial" //	4 bytes	autoincrementing integer	1 to 2147483647
				<< "bigserial" //8 bytes	large autoincrementing integer	1 to 9223372036854775807
				<< "varchar" //(n)	variable-length with limit
				<< "character varying"
				<< "char" //(n)	fixed-length, blank padded
				<< "text" //	variable unlimited length
				<< "boolean" //.
				<< "date" //	4 bytes	dates only	4713 BC	32767 AD	1 day
				<< "time" // [ (p) ] [ without time zone ]	8 bytes	times of day only	00:00:00	24:00:00	1 microsecond / 14 digits
				<< "timestamp" // [ (p) ] [ without time zone ]	8 bytes	both date and time	4713 BC	5874897 AD	1 microsecond / 14 digits
				<< "interval" // [ (p) ]	12 bytes	time intervals	-178000000 years	178000000 years	1 microsecond / 14 digits
				<< "money" //	4 bytes	currency amount	-21474836.48 to +21474836.47;
				<< "bytea" //	4 bytes plus the actual binary string	variable-length binary string
				<< "bit" //(n) and
				<< "bit varying" //(n)
				<< "cidr" //	12 or 24 bytes	IPv4 and IPv6 networks
				<< "inet" //	12 or 24 bytes	IPv4 and IPv6 hosts and networks
				<< "macaddr" ;//	6 bytes	MAC addresses
	}
	else if(connection().driverName().endsWith("MYSQL")) {
		types
				<< "boolean"
				<< "char"
				<< "varchar"
				<< "BINARY"
				<< "varbinary"
				<< "tinyint"
				<< "smallint"
				<< "mediumint"
				<< "int"
				<< "bigint"
				<< "float"
				<< "double"
				<< "decimal"
				<< "bit"
				<< "time"
				<< "date"
				<< "datetime"
				<< "timestamp"
				<< "year"
				<< "tinytext"
				<< "mediumtext"
				<< "text"
				<< "longtext"
				<< "tinyblob"
				<< "mediumblob"
				<< "blob"
				<< "longblob"
				<< "enum"
				<< "set";
	}
	lstType->addItems(types);
	clearFields();
	connect(lstCharacterSet, SIGNAL(activated(QString)), this, SLOT(on_lstCharacterSet_activated(QString)));
}

DlgColumnDef::~DlgColumnDef()
{
}

QSqlDatabase DlgColumnDef::connection()
{
	//qfTrash() << QF_FUNC_NAME;
	MainWindow *w = qfFindParent<MainWindow*>(this);
	return w->activeConnection();
}

void DlgColumnDef::enableControls(bool v)
{
	edName->setEnabled(v);
	lstType->setEnabled(v);
	edLength->setEnabled(v);
	edDecimals->setEnabled(v);
	edDefaultValue->setEnabled(v);
	chkNotNull->setEnabled(v);
	chkUnique->setEnabled(v);
	chkPrimaryKey->setEnabled(v);
	chkUnsigned->setEnabled(v);
	lstRefTable->setEnabled(v);
	lstRefColumn->setEnabled(v);
	lstCharacterSet->setEnabled(v);
	lstCollation->setEnabled(v);
	grpEnum->setEnabled(v);
	txtEnum->setEnabled(grpEnum->isEnabled());
	//qfInfo() << "1grpEnum enabled:" << grpEnum->isEnabled() << "txtEnum enabled:" << txtEnum->isEnabled();
}

void DlgColumnDef::clearFields()
{
	qfTrash() << QF_FUNC_NAME;
	edName->setText(QString());
	lstType->setCurrentIndex(0);
	edLength->setText(QString());
	edDecimals->setText(QString());
	edDefaultValue->setText(QString());
	chkNotNull->setChecked(false);
	chkUnique->setChecked(false);
	chkPrimaryKey->setChecked(false);
	chkUnsigned->setChecked(false);
	lstRefTable->clear();
	if(!dbName.isEmpty()) {
		QFSqlDbInfo di = connection().catalog().database(dbName);
		lstRefTable->addItems(di.tables());
	}
	lstRefTable->setCurrentIndex(-1);
	lstCharacterSet->setCurrentIndex(0);
	lstCharacterSet->setCurrentIndex(0);
}

void DlgColumnDef::loadColumnDefinition(const QFSqlFieldInfo &fi)
{
	qfTrash() << QF_FUNC_NAME;
	qfTrash() << fi.toString();
	clearFields();
	enableControls(true);
	edName->setFocus();
	edName->setText("new_field");
	edName->selectAll();
	btOk->setEnabled(true);
	if(!fi.isValid()) return;
	enableControls(false);
	edName->setText(fi.fieldName());
	edName->selectAll();
	lstCharacterSet->setEnabled(false);
	lstCharacterSet->setCurrentIndex(-1);
	lstCollation->setEnabled(false);
	lstCollation->setCurrentIndex(-1);
	QString s = fi.nativeType();
	qfTrash() << "\tnative type:" << s << "length:" << fi.length();
	if(connection().driverName().endsWith("MYSQL")) {
		if(s == "tinyint" && fi.length() == 1) s = "boolean";
	}
	int ix = lstType->findText(s, Qt::MatchExactly);
	lstType->setCurrentIndex(ix);
	edLength->setText(QString::number(fi.length()));
	if(fi.type() == QVariant::String) edDecimals->setText(QString());
	else edDecimals->setText(QString::number(fi.precision()));
	edDefaultValue->setText(fi.defaultValue().toString()); edName->setEnabled(true);
	chkNotNull->setChecked(!fi.isNullable());
	chkUnsigned->setChecked(fi.isUnsigned());
	chkPrimaryKey->setChecked(fi.isPriKey());
	txtComment->setPlainText(fi.comment());
	if(connection().driverName().endsWith("SQLITE")) {
		QFMsgShowAgain::show(this,"SQLite ver 3.2.2 can only rename already created fields");
		edName->setEnabled(true);
	}
	else if(connection().driverName().endsWith("PSQL")) {
		QFMsgShowAgain::show(this,"Altering columns is not fully supported yet.");
		edName->setEnabled(true);
		lstType->setEnabled(true);
		edDefaultValue->setEnabled(true);
		chkNotNull->setEnabled(true);
		lstRefTable->setEnabled(true);
		lstRefColumn->setEnabled(true);
	}
	else if(connection().driverName().endsWith("MYSQL")) {
		edName->setEnabled(true);
		lstType->setEnabled(true);
		edDefaultValue->setEnabled(true);
		chkNotNull->setEnabled(true);
		lstRefTable->setEnabled(true);
		lstRefColumn->setEnabled(true);
		chkUnique->setEnabled(true);
		chkPrimaryKey->setEnabled(true);
		chkUnsigned->setEnabled(true);
		edLength->setEnabled(true);
		edDecimals->setEnabled(true);
		txtComment->setEnabled(true);
		{
			QString char_set = fi.characterSet();
			int ix = lstCharacterSet->findText(char_set, Qt::MatchFixedString);
			lstCharacterSet->setCurrentIndex(ix);
		}
		lstCharacterSet->setEnabled(fi.type() == QVariant::String);
		{
			loadCollationsForCurrentCharset();
			QString collation = fi.collation();
			int ix = lstCollation->findText(collation, Qt::MatchFixedString);
			lstCollation->setCurrentIndex(ix);
		}
		lstCollation->setEnabled(fi.type() == QVariant::String);
		//qfInfo() << "2grpEnum enabled:" << grpEnum->isEnabled() << "txtEnum enabled:" << txtEnum->isEnabled();
		grpEnum->setEnabled(fi.nativeType() == "enum" || fi.nativeType() == "set");
		txtEnum->setPlainText(fi.enumOrSetFields().join("\n"));
		txtEnum->setEnabled(grpEnum->isEnabled());
		//qfInfo() << "3grpEnum enabled:" << grpEnum->isEnabled() << "txtEnum enabled:" << txtEnum->isEnabled();
	}
	//edOk->setEnabled(false);
}

void DlgColumnDef::on_lstRefTable_currentIndexChanged(const QString & text)
{
	lstRefColumn->clear();
	if(!text.isEmpty()) {
		QFSqlTableInfo ti = connection().catalog().table(QFSql::composeFullName(text, dbName), !Qf::ThrowExc);
		lstRefColumn->addItems(ti.unorderedFields());
	}
	lstRefColumn->setCurrentIndex(-1);
}

QString DlgColumnDef::toString()
{
	QString ret;
	QString type = lstType->currentText();
	ret = type;
	if(type == "enum" || type == "set") {
		QFString s = txtEnum->toPlainText();
		QStringList sl = s.splitAndTrim('\n');
		s = sl.join("','");
		ret += "('" + s + "')";
	}
	else {
		QString s = edLength->text();
		if(!s.isEmpty()) {
			ret += "(" + s;
			s = edDecimals->text().trimmed();
			if(!s.isEmpty() && s != "0") ret += "," + s;
			ret += ")";
		}
	}
	if(chkUnsigned->isChecked()) ret += " UNSIGNED";
	if(lstCharacterSet->isEnabled() && lstCharacterSet->currentIndex() > 0) ret += " CHARACTER SET " + lstCharacterSet->currentText();
	if(lstCollation->isEnabled() && lstCollation->currentIndex() > 0) ret += " COLLATE " + lstCollation->currentText();
	QString s = edDefaultValue->text();
	if(!s.isEmpty()) {
		ret += " DEFAULT " + s;
	}
	if(chkNotNull->isChecked()) ret += " NOT NULL";
	if(chkUnique->isChecked()) ret += " UNIQUE";

	if(connection().driverName().endsWith("SQLITE")) {
		/// sqlite umi jen prejmenovat sloupec, takze vrat nove jmeno
		//ret = edName->text();
	}
	else if(connection().driverName().endsWith("PSQL") ||
		        connection().driverName().endsWith("MYSQL")) {
		if(chkPrimaryKey->isChecked()) ret += " PRIMARY KEY";
		s = lstRefTable->currentText();
		if(!s.isEmpty()) {
			ret += " REFERENCES " + s;
			s = lstRefColumn->currentText();
			if(!s.isEmpty()) {
				ret += " (" + s + ")";
			}
		}
	}
	if(connection().driverName().endsWith("MYSQL")) {
		s = txtComment->toPlainText().trimmed();
		if(!s.isEmpty()) ret += " COMMENT '" + s + "'";
	}
	else {
		ret = QString();
	}
	return ret;
}


bool DlgColumnDef::showCommand()
{
	return chkShowCommand->isChecked();
}

void DlgColumnDef::loadCollationsForCurrentCharset()
{
	qfLogFuncFrame();
	lstCollation->clear();
	if(lstCharacterSet->isEnabled() && lstCharacterSet->currentIndex() > 0) {
		QString char_set = lstCharacterSet->currentText().toLower();
		QString drv_key;
		if(connection().driverName().endsWith("MYSQL")) {
			drv_key = "MYSQL";
		}
		if(!drv_key.isEmpty() && !char_set.isEmpty()) {
			if(!f_collationsCache.contains(drv_key)) {
				if(drv_key == "MYSQL") {
					f_collationsCache[drv_key]; /// zabran opakovanemu reloadu
					QFSqlQuery q(connection());
					q.exec("SELECT * FROM information_schema.collations", !Qf::ThrowExc);
					while(q.next()) {
						f_collationsCache[drv_key][q.value("character_set_name").toString().toLower()] << q.value("collation_name").toString().toLower();
					}
				}
			}
			QStringList collations = f_collationsCache[drv_key][char_set];
			lstCollation->addItem(trUtf8("default"));
			foreach(QString collation, collations) {
				lstCollation->addItem(collation);
			}
		}
	}
	else {
		lstCollation->addItem(trUtf8("default"));
		lstCollation->setCurrentIndex(0);
	}
}

void DlgColumnDef::on_lstCharacterSet_activated(const QString & text)
{
	Q_UNUSED(text);
	loadCollationsForCurrentCharset();
}
