
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#include "tableviewwidget.h"

#include <qfaction.h>
#include <qfsqltableview.h>
#include <qfcsvimportdialogwidget.h>
#include <qfbuttondialog.h>
#include <qfdlgexception.h>

#include <QtUiTools>
#include <QHBoxLayout>
#include <QCheckBox>

#include <qflogcust.h>

//======================================================
//                        TableView
//======================================================
class  TableView : public QFSqlTableView
{
	protected:
		//virtual void importCSV();
		//virtual QString exportReportDialogXmlPersistentId();
	public:
		TableView(QWidget *parent = NULL) : QFSqlTableView(parent) {
			//setSaveSettingsPersistentId("qsqlmon");
		}
};
/*
QString TableView::exportReportDialogXmlPersistentId()
{
	QString pers_id = "tableView/QFTablePrintDialogWidget";
	return pers_id;
}
*/
#if 0
//! qsqlmon ma extra options ohledne BLOBu, ale asi je to zbytecny a chtelo by to mit jen jednu funkci v QFTableView, ktera bude podporovat i bloby
void TableView::importCSV()
{
	qfLogFuncFrame();
	try {
		QFCSVImportDialogWidget *w = new QFCSVImportDialogWidget();
		{
			QUiLoader loader;
			QFile file(":/csvimportextraoptions.ui");
			file.open(QFile::ReadOnly);
			QWidget *w2 = loader.load(&file, w->extraOptionsFrame());
			file.close();

			QBoxLayout *ly = new QHBoxLayout(w->extraOptionsFrame());
			ly->setMargin(0);
			ly->addWidget(w2);
			w->extraOptionsFrame()->setVisible(true);
		}
		QFButtonDialog dlg;
		dlg.setXmlConfigPersistentId("QFTableView/importCSV/Dialog");
		dlg.setDialogWidget(w);
		QFCSVImportDialogWidget::ColumnMappingList lst;
		QFBasicTable *t = table();
		{
			foreach(const QFSqlField &fld, t->fields()) {
				lst << QFCSVImportDialogWidget::ColumnMapping(fld.fullName());
			}
		}
		w->setColumnMapping(lst);
		if(dlg.exec()) {
			bool load_blobs = false;
			{
				QCheckBox *cbx = w->extraOptionsFrame()->findChild<QCheckBox*>("chkLoadBlobs");
				load_blobs = cbx->isChecked();
			}
			QFBasicTable *t2 = w->table();
			lst = w->columnMapping();
			foreach(QFBasicTable::Row r2, t2->rows()) {
					//qfInfo() << r.toString();
				QFBasicTable::Row r = t->appendRow();
				int dest_colno = 0;
				foreach(const QFCSVImportDialogWidget::ColumnMapping& cm, lst) {
					int src_colno = cm.columnIndex();
					if(src_colno >= 0) {
						//QString colname = cm.columnName();
						QVariant val = r2.value(src_colno);
						if(load_blobs) {
							if(t->field(dest_colno).type() == QVariant::ByteArray) {
								QString file_name = val.toString();
								QFile f(file_name);
								if(f.open(QFile::ReadOnly)) {
									val = f.readAll();
								}
							}
						}
						r.setValue(dest_colno, val);
					}
					dest_colno++;
				}
				r.post();
			}
			reload();
		}
	}
	catch(QFException &e) {
		QFDlgException::exec(e);
	}
}
#endif
//======================================================
//                        TableViewWidget
//======================================================
class TableViewFactory1 : public QFTableViewWidget::TableViewFactory
{
	public:
		virtual QFTableView* createView() const {
			//qfInfo() << "tttttttttttttttttttttttttttt";
			return new TableView();
		}
};

//======================================================
//                        TableViewWidget
//======================================================
TableViewWidget::TableViewWidget(QWidget *parent)
	: QFTableViewWidget(parent, TableViewFactory1())
{
	//qfInfo() << "aaaaaaaaaaaaaaaaaa";
	setToolBarVisible(true);
	tableView()->setContextMenuActions(tableView()->contextMenuActionsForGroups(QFTableView::AllActions));
	tableView()->setCopyRowActionVisible(true);
}



