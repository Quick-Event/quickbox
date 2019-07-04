#include "lentcardswidget.h"
#include "ui_lentcardswidget.h"

#include <qf/core/model/sqltablemodel.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

LentCardsWidget::LentCardsWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::LentCardsWidget)
{
	setTitle(tr("Cards to rent"));
	setPersistentSettingsId("LentCardsWidget");
	ui->setupUi(this);
	ui->tblCardsTB->setTableView(ui->tblCards);
	{
		ui->tblCards->setPersistentSettingsId("tableView");
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		m->addColumn("siId", tr("SI"));
		m->addColumn("ignored", tr("Ignored"));
		m->addColumn("note", tr("Note"));
		ui->tblCards->setTableModel(m);
		m_tableModel = m;
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("lentcards", "*")
				.from("lentcards")
				.orderBy("siid");//.limit(10);
		m_tableModel->setQueryBuilder(qb, false);
		m_tableModel->reload();
	}
}

LentCardsWidget::~LentCardsWidget()
{
	delete ui;
}
