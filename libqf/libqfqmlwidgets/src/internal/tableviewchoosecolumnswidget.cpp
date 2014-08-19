#include "tableviewchoosecolumnswidget.h"
#include "ui_tableviewchoosecolumnswidget.h"

#include <qf/core/model/tablemodel.h>
#include <qf/core/log.h>

#include <QStandardItemModel>

using namespace qf::qmlwidgets::internal;

//=================================================
//             TableViewChooseColumnsWidget
//=================================================
TableViewChooseColumnsWidget::TableViewChooseColumnsWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::TableViewChooseColumnsWidget;
	ui->setupUi(this);

	QStyle *sty = style();
	ui->btColumnShiftUp->setIcon(sty->standardIcon(QStyle::SP_ArrowUp));
	ui->btColumnShiftDown->setIcon(sty->standardIcon(QStyle::SP_ArrowDown));
	ui->btColumnsNone->setIcon(sty->standardIcon(QStyle::SP_DialogCancelButton));
	ui->btColumnsAll->setIcon(sty->standardIcon(QStyle::SP_DialogOkButton));

	//Qf::connectSlotsByName(centralWidget(), this);
	f_exportedColumnsModel = new QStandardItemModel(this);
	f_exportedColumnsModel->setHorizontalHeaderLabels(QStringList() << trUtf8("name") << trUtf8("width"));
	ui->tblExportedColumns->setModel(f_exportedColumnsModel);
}

TableViewChooseColumnsWidget::~TableViewChooseColumnsWidget()
{
	delete ui;
}

void TableViewChooseColumnsWidget::loadColumns(qf::core::model::TableModel *model)
{
	if(model) {
		{
			QListWidget *w = ui->lstModelColumns;
			for(int i=0; i<model->columnCount(); i++) {
				QString caption = model->headerData(i, Qt::Horizontal).toString();
				QListWidgetItem *it = new QListWidgetItem(caption);
				it->setData(Qt::UserRole, "model");
				it->setData(Qt::UserRole+1, i);
				it->setFlags(it->flags() | Qt::ItemIsEditable);
				//it->setCheckState(Qt::Unchecked);
				w->addItem(it);
			}
		}
		const qf::core::utils::Table &tbl = model->table();
		{
			QListWidget *w = ui->lstTableColumns;
			int i = 0;
			for(const qf::core::utils::Table::Field &fld : tbl.fields()) {
				/// pokud je field soucasti modelu
				QListWidgetItem *it = new QListWidgetItem(fld.name());
				it->setData(Qt::UserRole, "table");
				it->setData(Qt::UserRole+1, i++);
				it->setFlags(it->flags() | Qt::ItemIsEditable);
				//it->setCheckState(Qt::Unchecked);
				w->addItem(it);
			}
		}
	}
}

void TableViewChooseColumnsWidget::on_btColumnsAll_clicked()
{
	QListWidget *w = ui->lstModelColumns->isVisible()? ui->lstModelColumns: ui->lstTableColumns;
	for(int i=0; i<w->count(); i++) w->item(i)->setSelected(true);
}

void TableViewChooseColumnsWidget::on_btColumnsNone_clicked()
{
	QListWidget *w = ui->lstModelColumns->isVisible()? ui->lstModelColumns: ui->lstTableColumns;
	for(int i=0; i<w->count(); i++) w->item(i)->setSelected(false);
}

void TableViewChooseColumnsWidget::on_btColumnsInvert_clicked()
{
	QListWidget *w = ui->lstModelColumns->isVisible()? ui->lstModelColumns: ui->lstTableColumns;
	for(int i=0; i<w->count(); i++) w->item(i)->setSelected(!w->item(i)->isSelected());
}

void TableViewChooseColumnsWidget::on_btColumnsAppendSelected_clicked()
{
	QListWidget *w = ui->lstModelColumns->isVisible()? ui->lstModelColumns: ui->lstTableColumns;
	QStandardItemModel *m = f_exportedColumnsModel;
	int row = m->rowCount();
	for(int i=0; i<w->count(); i++) {
		QListWidgetItem *it = w->item(i);
		if(it->isSelected()) {
			QString fld_name = it->text();
			QString origin = it->data(Qt::UserRole).toString();
			if(origin == "table")
				qf::core::Utils::parseFieldName(fld_name, &fld_name);
			QStandardItem *it2 = new QStandardItem(fld_name);
			it2->setData(it->data(Qt::UserRole), Qt::UserRole);
			it2->setData(it->data(Qt::UserRole+1), Qt::UserRole+1);
			m->setItem(row, 0, it2);
			it2 = new QStandardItem("%");
			m->setItem(row, 1, it2);
			row++;
		}
	}
	ui->tblExportedColumns->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void TableViewChooseColumnsWidget::on_btColumnsRemoveSelected_clicked()
{
	QStandardItemModel *m = f_exportedColumnsModel;
	QModelIndexList lst = ui->tblExportedColumns->selectionModel()->selectedIndexes();
	QMap<int, int> sel_rows; /// potrebuju jenom key, value me nezajma
	foreach(QModelIndex ix, lst) sel_rows[ix.row()];
	int del_cnt = 0;
	foreach(int row_ix, sel_rows.keys()) {
		m->removeRow(row_ix-del_cnt);
		del_cnt++;
	}
}

void TableViewChooseColumnsWidget::on_btColumnShiftUp_clicked()
{
	QModelIndex ix = ui->tblExportedColumns->currentIndex();
	int row = ix.row();
	if(row > 0) {
		QStandardItemModel *m = f_exportedColumnsModel;
		QList<QStandardItem*> lst = m->takeRow(row);
		row--;
		m->insertRow(row, lst);
		ix = ix.sibling(row, ix.column());
		ui->tblExportedColumns->setCurrentIndex(ix);
	}
}

void TableViewChooseColumnsWidget::on_btColumnShiftDown_clicked()
{
	QStandardItemModel *m = f_exportedColumnsModel;
	QModelIndex ix = ui->tblExportedColumns->currentIndex();
	int row = ix.row();
	if(row >= 0 && row < m->rowCount()-1) {
		QList<QStandardItem*> lst = m->takeRow(row);
		row++;
		m->insertRow(row, lst);
		ix = ix.sibling(row, ix.column());
		ui->tblExportedColumns->setCurrentIndex(ix);
	}
}

QVariantList TableViewChooseColumnsWidget::exportedColumns() const
{
	QVariantList ret;
	QStandardItemModel *m = f_exportedColumnsModel;
	for(int i=0; i<m->rowCount(); i++) {
		QStandardItem *it = m->item(i, 0);
		QVariantMap mp;
		mp["caption"] = it->text();
		mp["origin"] = it->data(Qt::UserRole);
		mp["index"] = it->data(Qt::UserRole+1);
		if(m->columnCount() > 1) {
			it = m->item(i, 1);
			mp["width"] = it->text();
		}
		ret << mp;
	}
	return ret;
}

void TableViewChooseColumnsWidget::setExportedColumns(const QVariant& cols)
{
	//QVariantList columns = cols.toList();
	QStandardItemModel *m = f_exportedColumnsModel;
	m->clear();
	int row = 0;
	foreach(QVariant v, cols.toList()) {
		QVariantMap mp = v.toMap();
		QString origin = mp.value("origin").toString();
		int index = mp.value("index").toInt();
		if(origin == "table") {
			if(index < 0 || index >= ui->lstTableColumns->count()) continue;
		}
		else if(origin == "model") {
			if(index < 0 || index >= ui->lstModelColumns->count()) continue;
		}
		QStandardItem *it2 = new QStandardItem(mp.value("caption").toString());
		it2->setData(origin, Qt::UserRole);
		it2->setData(mp.value("index"), Qt::UserRole+1);
		m->setItem(row, 0, it2);
		it2 = new QStandardItem(mp.value("width", "%").toString());
		m->setItem(row, 1, it2);
		row++;
	}
	ui->tblExportedColumns->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
