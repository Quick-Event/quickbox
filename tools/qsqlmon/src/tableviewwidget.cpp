#include "tableviewwidget.h"
#include "ui_tableviewwidget.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/tableview.h>

//#include <QtUiTools>
//#include <QHBoxLayout>
//#include <QCheckBox>
//#include <QApplication>

namespace qfq = qf::qmlwidgets;

//======================================================
//                        TableView
//======================================================
class  TableView : public qf::qmlwidgets::TableView
{
	protected:
		//virtual void importCSV();
		//virtual QString exportReportDialogXmlPersistentId();
	public:
		TableView(QWidget *parent = NULL) : qfq::TableView(parent)
		{
			//setSaveSettingsPersistentId("qsqlmon");
		}
};

//======================================================
//                        TableViewWidget
//======================================================
TableViewWidget::TableViewWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TableViewWidget)
{
	ui->setupUi(this);

	ui->btMenu->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
	connect(ui->btMenu, &QToolButton::clicked, [this](bool) {
		emit statusBarAction(this->ui->edInfo->text());
	});

	ui->toolBar->setTableView(ui->tableView);

	//ui->tableView->setContextMenuActions(tableView()->contextMenuActionsForGroups(qfq::TableView::AllActions));
	//ui->tableView->setCopyRowActionVisible(true);
}

TableViewWidget::~TableViewWidget()
{
	delete ui;
}

qf::qmlwidgets::TableView *TableViewWidget::tableView()
{
	return ui->tableView;
}

void TableViewWidget::updateStatus()
{
	qf::core::model::TableModel *m = tableView()->tableModel();
	if(m) {
		ui->lblRowCnt->setText(QString("%1 rows").arg(m->rowCount()));
	}
	tableView()->refreshActions();
}

void TableViewWidget::setInfo(const QString &info)
{
	ui->edInfo->setText(info);
}





