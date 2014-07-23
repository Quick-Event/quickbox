//
// C++ Implementation: dlgaltertable
//
// Description:
//
//
// Author: Fanda Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QDialog>
#include <QErrorMessage>

#include <qfmessage.h>
#include <qfsqlquery.h>
#include <qfsqlcatalog.h>
#include <qfdlgexception.h>
#include <qfdlgtextview.h>

#include "mainwindow.h"
#include "dlgaltertable.h"
#include "dlgcolumndef.h"
#include "dlgindexdef.h"

#include <qflogcust.h>

DlgAlterTable::DlgAlterTable(QWidget * parent, const QString& db, const QString& table)
	: QDialog(parent), tablename(table), dbname(db)
{
	setupUi(this);
	txtComment->setEnabled(false);
	if(connection().driverName().endsWith("SQLITE")) {
		btFieldInsert->setEnabled(false);
		btFieldDelete->setEnabled(false);
		btFieldEdit->setEnabled(false); /// It is not possible to rename a colum, remove a column, or add or remove constraints from a table.
	}
	else if(connection().driverName().endsWith("MYSQL")) {
		txtComment->setEnabled(true);
	}
	refresh();
}

DlgAlterTable::~DlgAlterTable()
{
}

void DlgAlterTable::refresh()
{
	connection().catalog().database(dbname).forgetTable(tablename);
	//lstFields->addItems(connection.catalog().database(dbname).table(tablename).unorderedFields());
	QFSqlTableInfo ti = connection().catalog() .table(dbname + "." + tablename);
	qfTrash() << "DlgAlterTable::refresh() ti is valid:" << ti.isValid();
	lstFields->clear();
	lstFields->addItems(ti.unorderedFields());
	lstIndexes->clear();
	foreach(QFSqlConnectionBase::IndexInfo ii, ti.indexes()) lstIndexes->addItem(ii.name);
	if(connection().driverName().endsWith("MYSQL")) {
		QFSqlQuery q(connection());
		q.exec(QString("SHOW TABLE status FROM %1 LIKE '%2'").arg(dbname).arg(tablename));
		if(q.next()) {
			oldComment = q.value("comment").toString();
			txtComment->setPlainText(oldComment);
		}
	}
}

void DlgAlterTable::on_btFieldInsert_clicked(bool append)
{
	if(lstFields->currentRow() < 0) append = true;

	DlgColumnDef dlg(this, QFSql::composeFullName(tablename, dbname));
	while(true) {
		if(dlg.exec() == QDialog::Accepted) {
			QStringList sql_commands;
			if(connection().driverName().endsWith("SQLITE")) {
				if(!append) {
					QFMessage::information(this, "Not supported in SQLite version <= 3.2.2");
				}
				QString fld_name = dlg.edName->text();
				QString s, qs = "ALTER TABLE %1 ADD COLUMN %2 %3";
				qs = qs.arg(tablename).arg(fld_name).arg(dlg.lstType->currentText());
				qs += dlg.toString();
				sql_commands << qs;
				//if(!execCommand(qs)) continue;
				sql_commands << ("VACUUM " + tablename);
				//refresh();
			}
			else if(connection().driverName().endsWith("PSQL")) {
				if(!append) {
					QFMessage::information(this, "Not supported in PSQL version <= 8.1.0");
				}
				QString fld_name = dlg.edName->text();
				QString s, qs = "ALTER TABLE %1.%2 ADD COLUMN \"%3\" %4";
				qs = qs.arg(dbname).arg(tablename).arg(fld_name).arg(dlg.lstType->currentText());
				qs += dlg.toString();
				sql_commands << qs;
				//if(!execCommand(qs)) continue;
				//refresh();
			}
			else if(connection().driverName().endsWith("MYSQL")) {
				QString insert_where;
				if(!append) {
					int row = lstFields->currentRow() - 1;
					if(row < 0) insert_where = " FIRST";
					else {
						QString s = lstFields->item(row)->text();
						insert_where = " AFTER " + s;
					}
				}
				QString fld_name = dlg.edName->text();
				QString s, qs = "ALTER TABLE %1 ADD COLUMN %2 ";
				qs = qs.arg(tablename).arg(fld_name);
				qs += dlg.toString();
				qs += insert_where;
				sql_commands << qs;
				//if(!execCommand(qs)) continue;
				//refresh();
			}
			else {
				QFMessage::information(this, "Not supported yet.");
				continue;
			}
			if(!sql_commands.isEmpty()) {
				QString s = sql_commands.join(";\n");
				bool ok = true;
				if(dlg.showCommand()) {
					ok = QFDlgTextView::exec(this, s, QString(), "dlgShowCommand");
				}
				if(ok) if(!execCommand(s)) continue;
				refresh();
			}
		}
		break;
	}
}

void DlgAlterTable::on_btFieldAppend_clicked()
{
	on_btFieldInsert_clicked(true);
}

void DlgAlterTable::on_btFieldEdit_clicked()
{
	qfTrash() << QF_FUNC_NAME;
	if(lstFields->currentRow() < 0) return;

	QString full_table_name = QFSql::composeFullName(tablename, dbname);
	//qfTrash() << "\ts:" << s;
	DlgColumnDef dlg(this, full_table_name);
	QString fld_name = lstFields->currentItem()->text();
	//qfTrash() << "\t####################################";
	QFSqlFieldInfo fi = connection().catalog().table(full_table_name).field(fld_name);
	//qfTrash() << "\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4";
	dlg.loadColumnDefinition(fi);
	while(true) {
		if(dlg.exec() == QDialog::Accepted) {
			QStringList sql_commands;
			if(connection().driverName().endsWith("SQLITE")) {
				//qfInfo() << fi.fieldName() << dlg.edName->text();
				if(fi.fieldName() != dlg.edName->text()) {
					QString qs = "ALTER TABLE %1 RENAME COLUMN %2 %3";
					qs = qs.arg(tablename).arg(fi.fieldName()).arg(dlg.edName->text());
					sql_commands << qs;
					//if(!execCommand(qs)) continue;
					//refresh();
				}
			}
			else if(connection().driverName().endsWith("PSQL")) {
				if(fi.fieldName() != dlg.edName->text()) {
					QString s = "ALTER TABLE %1 RENAME COLUMN \"%2\" TO \"%3\"";
					s = s.arg(tablename).arg(fi.fieldName()).arg(dlg.edName->text());
					sql_commands << s;
					//if(!execCommand(s)) continue;
					//refresh();
				}
				QStringList actions;
				if(fi.nativeType() != dlg.lstType->currentText()) {
					QFString s = "ALTER COLUMN \"" + dlg.edName->text() + "\" TYPE " + dlg.lstType->currentText();
					actions << s;
				}
				if(fi.defaultValue().toString() != dlg.edDefaultValue->text()) {
					QFString s1 = dlg.edDefaultValue->text().trimmed();
					QFString s = "ALTER COLUMN \"" + dlg.edName->text() + "\"";
					if(!!s1) s += " SET DEFAULT " + s1;
					else s += " DROP DEFAULT";
					actions << s;
				}
				if(fi.isNullable() == dlg.chkNotNull->isChecked()) {
					QFString s = "ALTER COLUMN \"%1\" %2 NOT NULL";
					s = s.arg(dlg.edName->text());
					s = s.arg((dlg.chkNotNull->isChecked())? "SET": "DROP");
					actions << s;
				}
				if(!actions.isEmpty()) {
					QString s = "ALTER TABLE " + tablename + " " + actions.join(", ");
					sql_commands << s;
					//if(!execCommand(s)) continue;
					//refresh();
				}
			}
			else if(connection().driverName().endsWith("MYSQL")) {
				QString s = "ALTER TABLE %1 CHANGE COLUMN %2";
				s = s.arg(tablename).arg(fi.fieldName());
				s += " " + dlg.edName->text();
				s += " " + dlg.toString();
				sql_commands << s;
				//if(!execCommand(s)) continue;
				//refresh();
			}
			else {
				QFMessage::information(this, "Not supported yet.");
				continue;
			}
			if(!sql_commands.isEmpty()) {
				QString s = sql_commands.join(";\n");
				bool ok = true;
				if(dlg.showCommand()) {
					ok = QFDlgTextView::exec(this, s, QString(), "dlgShowCommand");
				}
				if(ok) if(!execCommand(s)) continue;
				refresh();
			}
		}
		connection().catalog().forgetTable(full_table_name);
		break;
	}
}


void DlgAlterTable::on_btFieldDelete_clicked()
{
	if(lstFields->currentRow() < 0) return;

	QString fld_name = lstFields->currentItem()->text();
	if(QFMessage::askYesNo(this, tr("Realy drop column '%1'").arg(fld_name))) {
		QString s = "ALTER TABLE %1 DROP COLUMN %2";
		s = s.arg(tablename).arg(fld_name);
		execCommand(s);
		refresh();
	}
}

MainWindow* DlgAlterTable::mainWindow()
{
	MainWindow *w = qfFindParent<MainWindow*>(this);
	return w;
}

QFSqlConnection DlgAlterTable::connection()
{
	//qfTrash() << QF_FUNC_NAME;
	return mainWindow()->activeConnection();
}

bool DlgAlterTable::execCommand(const QString &qs)
{
	return mainWindow()->execCommand(qs);
}

void DlgAlterTable::on_btIndexAdd_clicked()
{
	DlgIndexDef dlg(this, dbname + "." + tablename);
	if(dlg.exec()) 	{
		execCommand(dlg.createIndexCommand());
		refresh();
	}
}

void DlgAlterTable::on_btIndexEdit_clicked()
{
	if(lstIndexes->currentRow() < 0) return;
	DlgIndexDef dlg(this, dbname + "." + tablename, lstIndexes->currentItem()->text());
	if(dlg.exec()) 	{
		QString indexname = lstIndexes->currentItem()->text();
		if(connection().driverName().endsWith("SQLITE")) { execCommand("DROP INDEX " + indexname); }
		else { execCommand("DROP INDEX " + indexname + " ON " + dbname + "." + tablename); }
		execCommand(dlg.createIndexCommand());
		refresh();
	}
	//QFMessage::information(this, tr("If you want to edit index, drop it and create again."));
}

void DlgAlterTable::on_btIndexDelete_clicked()
{
	if(lstIndexes->currentRow() < 0) return;
	QString indexname = lstIndexes->currentItem()->text();
	if(QFMessage::askYesNo(this, tr("Do you realy want to drop index %1?").arg(indexname), true)) {
		if(connection().driverName().endsWith("SQLITE")) { execCommand("DROP INDEX " + indexname); }
		else { execCommand("DROP INDEX " + indexname + " ON " + dbname + "." + tablename); }
		refresh();
	}
}

void DlgAlterTable::on_btOk_clicked()
{
	try {
		if(connection().driverName().endsWith("MYSQL")) {
			QString new_comment = txtComment->toPlainText();
			if(new_comment != oldComment) {
				QFSqlQuery q(connection());
				q.exec(QString("ALTER TABLE %1.%2 COMMENT='%3'").arg(dbname).arg(tablename).arg(new_comment));
			}
		}
		accept();
	}
	catch(QFException &e) {QFDlgException::exec(this, e);}
}
