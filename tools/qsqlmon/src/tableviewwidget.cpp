
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#include "tableviewwidget.h"

//#include <qfaction.h>
//#include <qfsqltableview.h>
//#include <qfcsvimportdialogwidget.h>
//#include <qfbuttondialog.h>
//#include <qfdlgexception.h>

#include <qf/core/log.h>
#include <qf/qmlwidgets/tableview.h>

//#include <QtUiTools>
#include <QHBoxLayout>
#include <QCheckBox>

//======================================================
//                        TableView
//======================================================
class  TableView : public qf::qmlwidgets::TableView
{
	protected:
		//virtual void importCSV();
		//virtual QString exportReportDialogXmlPersistentId();
	public:
		TableView(QWidget *parent = NULL) : QFSqlTableView(parent) {
			//setSaveSettingsPersistentId("qsqlmon");
		}
};

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



