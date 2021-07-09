#include "stationsbackupmemorywidget.h"
#include "ui_stationsbackupmemorywidget.h"

#include <qf/core/model/sqltablemodel.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

StationsBackupMemoryWidget::StationsBackupMemoryWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::StationsBackupMemoryWidget)
{
	setTitle(tr("Stations backup memory"));
	setPersistentSettingsId("StationsBackupMemoryWidget");
	ui->setupUi(this);
	ui->tblCardsTB->setTableView(ui->tblCards);
	{
		ui->tblCards->setPersistentSettingsId("tableView");
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		m->addColumn("stageId", tr("Stage"));
		m->addColumn("stationNumber", tr("Station number"));
		m->addColumn("siId", tr("SI"));
		m->addColumn("punchDateTime", tr("Punch time"));
		m->addColumn("cardErr", tr("Card error"));
		ui->tblCards->setTableModel(m);
		m_tableModel = m;
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("stationsbackup", "*")
				.from("stationsbackup")
				.orderBy("stageId, punchDateTime");//.limit(10);
		m_tableModel->setQueryBuilder(qb, false);
		m_tableModel->reload();
	}
}

StationsBackupMemoryWidget::~StationsBackupMemoryWidget()
{
	delete ui;
}
