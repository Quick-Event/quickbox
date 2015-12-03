#include "mainwindow.h"
#include "theapp.h"

#include "ui_dlgindexdef.h"
#include "dlgindexdef.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>
#include <qf/core/string.h>
#include <qf/core/sql/catalog.h>

namespace qfc = qf::core;

DlgIndexDef::DlgIndexDef(QWidget * parent, const QString &table_name, const QString &index_name)
	: QDialog(parent), indexName(index_name)
{
	ui = new Ui::DlgIndexDef;
	ui->setupUi(this);
	ui->btAdd->setDefaultAction(ui->actionAddFieldToIndex);
	ui->btRemove->setDefaultAction(ui->actionRemoveFieldFromIndex);

	qf::core::Utils::parseFieldName(table_name, &tableName, &dbName);

	ui->edIndexName->setText(indexName);
	//DlgAlterTable *dat = qfFindInheritedParent<DlgAlterTable*>(this);
	//qf::core::sql::DbInfo dbi(connection());
	qf::core::sql::FieldInfoList filst;
	filst.load(connection(), table_name);
	ui->lstTable->addItems(filst.unorderedKeys());
	loadIndexDefinition();
}

DlgIndexDef::~DlgIndexDef()
{
	delete ui;
}

QSqlDatabase DlgIndexDef::connection()
{
	//qfTrash() << QF_FUNC_NAME;
	MainWindow *w = qf::core::Utils::findParent<MainWindow*>(this);
	return w->activeConnection();
}

void DlgIndexDef::on_actionAddFieldToIndex_triggered()
{
	//qfTrash() << QF_FUNC_NAME;
	QListWidgetItem *it;
	if(ui->edIndexName->text().isEmpty()) {
		it = ui->lstTable->currentItem();
		if(it) {
			qfc::String s = it->text();
			ui->edIndexName->setText(tableName + "_x" + s.slice(0, 1).toUpper() + s.slice(1));
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
	QF_SAFE_DELETE(it);
}

void DlgIndexDef::loadIndexDefinition()
{
	QString s = "%1.%2";
	qfc::sql::Connection dbi(connection());
	qfc::sql::Connection::IndexList il = dbi.indexes(s.arg(dbName).arg(tableName));
	Q_FOREACH(qfc::sql::Connection::IndexInfo ii, il) {
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


