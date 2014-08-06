
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_columnselectorwidget.h"
#include "columnselectorwidget.h"

#include <qf/core/log.h>
#include <qf/core/sql/connection.h>

#include <QTimer>
#include <QListWidget>
#include <QStringBuilder>

#include <qclipboard.h>

//=================================================
//             ColumnSelectorWidget
//=================================================
ColumnSelectorWidget::ColumnSelectorWidget(QString table_name, const QSqlDatabase &conn, QWidget *parent)
	: qf::qmlwidgets::framework::PartWidget(parent), f_tableName(table_name)
{
	ui = new Ui::ColumnSelectorWidget;
	ui->setupUi(this);
	//Qf::connectSlotsByName(centralWidget(), this);
	{
		qf::core::sql::Connection dbi(conn);
		QStringList fields = dbi.fields(f_tableName);
		QListWidget *w = ui->lstFields;
		foreach(QString fld, fields) {
			QListWidgetItem *it = new QListWidgetItem(fld);
			//it->setCheckState(Qt::Unchecked);
			//it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
			w->addItem(it);
		}
	}
	setPersistentSettingsId("ColumnSelectorWidget");
	QTimer::singleShot(0, this, SLOT(lazyInit()));
}

ColumnSelectorWidget::~ColumnSelectorWidget()
{
	delete ui;
}

void ColumnSelectorWidget::lazyInit()
{
	//QFXmlConfigPersistenter::loadPersistentDataRecursively(this);
}

void ColumnSelectorWidget::on_btAll_clicked()
{
	QListWidget *w = ui->lstFields;
	w->selectAll();
}

void ColumnSelectorWidget::on_btInvert_clicked()
{
	QListWidget *w = ui->lstFields;
	QAbstractItemModel *m = w->model();
	QItemSelectionModel *sm = w->selectionModel();
	for(int i=0; i<m->rowCount(); i++) {
		QModelIndex ix = m->index(i, 0);
		QItemSelectionModel::SelectionFlags flgs = QItemSelectionModel::Toggle;
		sm->select(ix, flgs);
	}
}

void ColumnSelectorWidget::on_btPasteSelectedColumns_clicked()
{
	QStringList col_names;
	QListWidget *w = ui->lstFields;
	QAbstractItemModel *m = w->model();
	QItemSelectionModel *sm = w->selectionModel();
	for(int i=0; i<m->rowCount(); i++) {
		QModelIndex ix = m->index(i, 0);
		if(sm->isSelected(ix)) {
			QString s = m->data(ix).toString();
			if(ui->chkIncludeTableNames->isChecked()) s = f_tableName%'.'%s;
			col_names << s;
		}
	}
	if(!col_names.isEmpty()) {
		QString s = col_names.join(ui->edColumnSeparator->text());
		QClipboard *clipboard = QApplication::clipboard();
		if(ui->chkIncludeSelect->isChecked()) s = "SELECT "%s%" FROM "%f_tableName;
		clipboard->setText(s);
		emit columnNamesCopiedToClipboard(s);
	}
}
