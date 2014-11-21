#include "tableviewcopytodialogwidget.h"
#include "ui_tableviewcopytodialogwidget.h"
#include "../tableview.h"

#include <qf/core/log.h>
#include <qf/core/model/tablemodel.h>

namespace qfm = qf::core::model;
using namespace qf::qmlwidgets::internal;

TableViewCopyToDialogWidget::TableViewCopyToDialogWidget(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::TableViewCopyToDialogWidget;
	ui->setupUi(this);

	qfm::TableModel *m = new qfm::TableModel(this);
	//m->setElideDisplayedTextAt(0);
	//qfTrash() << QF_FUNC_NAME << "model created:" << f_model;
	tableView()->setModel(m);
}

TableViewCopyToDialogWidget::~TableViewCopyToDialogWidget()
{
	delete ui;
}
/*
QFTableViewWidget* TableViewCopyToDialogWidget::tableViewWidget()
{
	return ui->tableViewWidget;
}
*/
qf::qmlwidgets::TableView* TableViewCopyToDialogWidget::tableView()
{
	return ui->tableView;
}

bool TableViewCopyToDialogWidget::isInsert()
{
	return ui->chkInsertRows->isChecked();
}
