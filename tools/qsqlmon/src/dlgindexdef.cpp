
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
#include <qfsqlquery.h>
#include <qfmessage.h>

#include "mainwindow.h"
//#include "dlgaltertable.h"

#include "ui_dlgindexdef.h"
#include "dlgindexdef.h"

#include <qflogcust.h>

DlgIndexDef::DlgIndexDef(QWidget * parent, const QString &table_name, const QString &index_name)
	: QDialog(parent), indexName(index_name)
{
	ui = new Ui::DlgIndexDef;
	ui->setupUi(this);
	ui->btAdd->setDefaultAction(ui->actionAddFieldToIndex);
	ui->btRemove->setDefaultAction(ui->actionRemoveFieldFromIndex);

	QFSql::parseFullName(table_name, &tableName, &dbName);

	ui->edIndexName->setText(indexName);
	//DlgAlterTable *dat = qfFindInheritedParent<DlgAlterTable*>(this);
	QFSqlTableInfo ti = connection().catalog().table(dbName + "." + tableName);
	ui->lstTable->addItems(ti.unorderedFields());
	loadIndexDefinition();
}

DlgIndexDef::~DlgIndexDef()
{
	delete ui;
}

QFSqlConnection DlgIndexDef::connection()
{
	//qfTrash() << QF_FUNC_NAME;
	MainWindow *w = qfFindParent<MainWindow*>(this);
	return w->activeConnection();
}

void DlgIndexDef::on_actionAddFieldToIndex_triggered()
{
	//qfTrash() << QF_FUNC_NAME;
	QListWidgetItem *it;
	if(ui->edIndexName->text().isEmpty()) {
		it = ui->lstTable->currentItem();
		if(it) {
			QFString s = it->text();
			ui->edIndexName->setText("x" + s.slice(0, 1).toUpper() + s.slice(1));
		}
	}
	it = ui->lstTable->currentItem();
	if(it) {
		ui->lstIndex->addItem(it->clone());
	}
}

void DlgIndexDef::on_actionRemoveFieldFromIndex_triggered()
{
	//qfTrash() << QF_FUNC_NAME;
	QListWidgetItem *it = ui->lstIndex->currentItem();
	SAFE_DELETE(it);
}

void DlgIndexDef::loadIndexDefinition()
{
	QString s = "%1.%2";
	QFSqlConnectionBase::IndexList il = connection().indexes(s.arg(dbName).arg(tableName));
	foreach(QFSqlConnectionBase::IndexInfo ii, il) {
		if(ii.name == indexName) {
			ui->chkUnique->setChecked(ii.unique);
			foreach(s, ii.fields) ui->lstIndex->addItem(s);
			break;
		}
	}
}

QString DlgIndexDef::createIndexCommand()
{
	QString s = "CREATE %4 INDEX %1 ON %2 (%3)";
	s = s.arg(ui->edIndexName->text(), tableName);
	QStringList sl;
	for(int i=0; i<ui->lstIndex->count(); i++) sl << ui->lstIndex->item(i)->text();
	s = s.arg(sl.join(","));
	s = s.arg(ui->chkUnique->isChecked()? "UNIQUE": "");
	return s;
}


