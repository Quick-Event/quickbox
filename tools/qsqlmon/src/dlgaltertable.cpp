#include "mainwindow.h"
#include "theapp.h"
#include "dlgaltertable.h"
#include "dlgcolumndef.h"
#include "dlgindexdef.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>
#include <qf/core/sql/query.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/previewdialog.h>

#include <QDialog>
#include <QErrorMessage>
#include <QSqlQuery>

namespace qfc = qf::core;

DlgAlterTable::DlgAlterTable(QWidget * parent, const QString& db, const QString& table)
	: QDialog(parent), m_tableName(table), m_schemaName(db)
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
	qf::core::sql::FieldInfoList fldlst;
	fldlst.load(connection(), qfc::Utils::composeFieldName(m_tableName, m_schemaName));
	lstFields->clear();
	lstFields->addItems(fldlst.unorderedKeys());
	lstIndexes->clear();

	qf::core::sql::IndexInfoList ixlst;
	ixlst.load(connection(), qfc::Utils::composeFieldName(m_tableName, m_schemaName));
	lstIndexes->addItems(ixlst.unorderedKeys());

	if(connection().driverName().endsWith("MYSQL")) {
		QSqlQuery q(connection());
		q.exec(QString("SHOW TABLE status FROM %1 LIKE '%2'").arg(m_schemaName).arg(m_tableName));
		if(q.next()) {
			oldComment = q.value("comment").toString();
			txtComment->setPlainText(oldComment);
		}
	}
}

void DlgAlterTable::on_btFieldInsert_clicked(bool append)
{
	if(lstFields->currentRow() < 0) append = true;

	DlgColumnDef dlg(this, qfc::Utils::composeFieldName(m_tableName, m_schemaName));
	while(true) {
		if(dlg.exec() == QDialog::Accepted) {
			QStringList sql_commands;
			if(connection().driverName().endsWith("SQLITE")) {
				if(!append) {
					qf::qmlwidgets::dialogs::MessageBox::showInfo(this, "Not supported in SQLite version <= 3.2.2");
				}
				QString fld_name = dlg.edName->text();
				QString s, qs = "ALTER TABLE %1 ADD COLUMN %2";
				qs = qs.arg(m_tableName).arg(fld_name);
				qs += dlg.toString();
				sql_commands << qs;
				//if(!execCommand(qs)) continue;
				sql_commands << ("VACUUM " + m_tableName);
				//refresh();
			}
			else if(connection().driverName().endsWith("PSQL")) {
				if(!append) {
					qf::qmlwidgets::dialogs::MessageBox::showInfo(this, "Columns insertion is not supported in PSQL");
				}
				QString fld_name = dlg.edName->text();
				QString s, qs = "ALTER TABLE %1.%2 ADD COLUMN \"%3\"";
				qs = qs.arg(m_schemaName).arg(m_tableName).arg(fld_name);
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
				qs = qs.arg(m_tableName).arg(fld_name);
				qs += dlg.toString();
				qs += insert_where;
				sql_commands << qs;
				//if(!execCommand(qs)) continue;
				//refresh();
			}
			else {
				qf::qmlwidgets::dialogs::MessageBox::showInfo(this, "Not supported yet.");
				continue;
			}
			if(!sql_commands.isEmpty()) {
				QString s = sql_commands.join(";\n");
				bool ok = true;
				if(dlg.showCommand()) {
					ok = qf::qmlwidgets::dialogs::PreviewDialog::exec(this, s, QString(), "dlgShowCommand");
				}
				if(ok)
					if(!execCommand(s))
						continue;
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
	qfLogFuncFrame();
	if(lstFields->currentRow() < 0)
		return;

	QString full_table_name = qf::core::Utils::composeFieldName(m_tableName, m_schemaName);
	//qfDebug() << "\ts:" << s;
	DlgColumnDef dlg(this, full_table_name);
	QString fld_name = lstFields->currentItem()->text();
	qf::core::sql::FieldInfoList fldlst;
	fldlst.load(connection(), full_table_name);
	qf::core::sql::FieldInfo fi = fldlst.value(fld_name);
	dlg.loadColumnDefinition(fi);
	while(true) {
		if(dlg.exec() == QDialog::Accepted) {
			QStringList sql_commands;
			if(connection().driverName().endsWith("SQLITE")) {
				//qfInfo() << fi.fieldName() << dlg.edName->text();
				if(fi.shortName() != dlg.edName->text()) {
					QString qs = "ALTER TABLE %1 RENAME COLUMN %2 %3";
					qs = qs.arg(m_tableName).arg(fi.shortName()).arg(dlg.edName->text());
					sql_commands << qs;
					//if(!execCommand(qs)) continue;
					//refresh();
				}
			}
			else if(connection().driverName().endsWith("PSQL")) {
				if(fi.shortName() != dlg.edName->text()) {
					QString s = "ALTER TABLE %1 RENAME COLUMN \"%2\" TO \"%3\"";
					s = s.arg(m_tableName).arg(fi.shortName()).arg(dlg.edName->text());
					sql_commands << s;
					//if(!execCommand(s)) continue;
					//refresh();
				}
				QStringList actions;
				if(fi.nativeType() != dlg.lstType->currentText()) {
					QString s = "ALTER COLUMN \"" + dlg.edName->text() + "\" TYPE " + dlg.lstType->currentText();
					actions << s;
				}
				if(fi.defaultValue().toString() != dlg.edDefaultValue->text()) {
					QString s1 = dlg.edDefaultValue->text().trimmed();
					QString s = "ALTER COLUMN \"" + dlg.edName->text() + "\"";
					if(!s1.isEmpty()) s += " SET DEFAULT " + s1;
					else s += " DROP DEFAULT";
					actions << s;
				}
				if(fi.isNullable() == dlg.chkNotNull->isChecked()) {
					QString s = "ALTER COLUMN \"%1\" %2 NOT NULL";
					s = s.arg(dlg.edName->text());
					s = s.arg((dlg.chkNotNull->isChecked())? "SET": "DROP");
					actions << s;
				}
				if(!actions.isEmpty()) {
					QString s = "ALTER TABLE " + m_tableName + " " + actions.join(", ");
					sql_commands << s;
					//if(!execCommand(s)) continue;
					//refresh();
				}
			}
			else if(connection().driverName().endsWith("MYSQL")) {
				QString s = "ALTER TABLE %1 CHANGE COLUMN %2";
				s = s.arg(m_tableName).arg(fi.shortName());
				s += " " + dlg.edName->text();
				s += " " + dlg.toString();
				sql_commands << s;
				//if(!execCommand(s)) continue;
				//refresh();
			}
			else {
				qf::qmlwidgets::dialogs::MessageBox::showInfo(this, "Not supported yet.");
				continue;
			}
			if(!sql_commands.isEmpty()) {
				QString s = sql_commands.join(";\n");
				bool ok = true;
				if(dlg.showCommand()) {
					ok = qf::qmlwidgets::dialogs::PreviewDialog::exec(this, s, QString(), "dlgShowCommand");
				}
				if(ok)
					if(!execCommand(s))
						continue;
				refresh();
			}
		}
		//connection().catalog().forgetTable(full_table_name);
		break;
	}
}


void DlgAlterTable::on_btFieldDelete_clicked()
{
	if(lstFields->currentRow() < 0) return;

	QString fld_name = lstFields->currentItem()->text();
	if(qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Realy drop column '%1'").arg(fld_name))) {
		QString s = "ALTER TABLE %1 DROP COLUMN \"%2\"";
		s = s.arg(m_tableName).arg(fld_name);
		execCommand(s);
		refresh();
	}
}

MainWindow* DlgAlterTable::mainWindow()
{
	MainWindow *w = qfFindParent<MainWindow*>(this);
	return w;
}

QSqlDatabase DlgAlterTable::connection()
{
	//qfLogFuncFrame();
	return mainWindow()->activeConnection();
}

bool DlgAlterTable::execCommand(const QString &qs)
{
	return mainWindow()->execCommand(qs);
}

QString DlgAlterTable::dropIndexCommand(const QString &index_name)
{
	QString ret = "DROP INDEX " + index_name;
	if(connection().driverName().endsWith("MYSQL")) {
		ret += " ON " + m_schemaName + "." + m_tableName;
	}
	return ret;
}

void DlgAlterTable::on_btIndexAdd_clicked()
{
	DlgIndexDef dlg(this, m_schemaName + "." + m_tableName);
	if(dlg.exec()) 	{
		execCommand(dlg.createIndexCommand());
		refresh();
	}
}

void DlgAlterTable::on_btIndexEdit_clicked()
{
	if(lstIndexes->currentRow() < 0) return;
	DlgIndexDef dlg(this, m_schemaName + "." + m_tableName, lstIndexes->currentItem()->text());
	if(dlg.exec()) 	{
		QString indexname = lstIndexes->currentItem()->text();
		execCommand(dropIndexCommand(indexname));
		execCommand(dlg.createIndexCommand());
		refresh();
	}
	//qf::qmlwidgets::dialogs::MessageBox::showInfo(this, tr("If you want to edit index, drop it and create again."));
}

void DlgAlterTable::on_btIndexDelete_clicked()
{
	if(lstIndexes->currentRow() < 0) return;
	QString indexname = lstIndexes->currentItem()->text();
	if(qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Do you realy want to drop index %1?").arg(indexname), true)) {
		execCommand(dropIndexCommand(indexname));
		refresh();
	}
}

void DlgAlterTable::accept()
{
	if(connection().driverName().endsWith("MYSQL")) {
		QString new_comment = txtComment->toPlainText();
		if(new_comment != oldComment) {
			qf::core::sql::Query q(connection());
			bool ok = q.exec(QString("ALTER TABLE %1.%2 COMMENT='%3'").arg(m_schemaName).arg(m_tableName).arg(new_comment));
			if(!ok)
				qfError() << q.lastErrorText();
		}
	}
	Super::accept();
}

