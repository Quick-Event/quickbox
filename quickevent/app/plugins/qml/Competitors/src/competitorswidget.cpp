#include "competitorswidget.h"
#include "ui_competitorswidget.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfm = qf::core::model;

CompetitorsWidget::CompetitorsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorsWidget)
{
	ui->setupUi(this);

	ui->tblCompetitorsToolBar->setTableView(ui->tblCompetitors);

	ui->tblCompetitors->setPersistentSettingsId("tblCompetitors");
	ui->tblCompetitors->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblCompetitors->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("siId", tr("SI"));
	qfs::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("competitors")
			.join("competitors.classId", "classes.id")
			.orderBy("competitors.id");//.limit(100);
	m->setQueryBuilder(qb);
	ui->tblCompetitors->setTableModel(m);
	m_competitorsModel = m;

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

CompetitorsWidget::~CompetitorsWidget()
{
	delete ui;
}

void CompetitorsWidget::lazyInit()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	connect(fwk->plugin("Event"), SIGNAL(eventOpenChanged(bool)), this, SLOT(onEventOpenChanged(bool)));
}

void CompetitorsWidget::onEventOpenChanged(bool open)
{
	m_competitorsModel->reload();
}
