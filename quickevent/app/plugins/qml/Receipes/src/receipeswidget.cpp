#include "receipeswidget.h"
#include "ui_receipeswidget.h"
#include "receipespartwidget.h"
#include "Receipes/receipesplugin.h"

#include <Event/eventplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/model/sqltablemodel.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>
#include <QMetaObject>
#include <QJSValue>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

const char *ReceipesWidget::SETTINGS_PREFIX = "plugins/Receipes";

ReceipesWidget::ReceipesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ReceipesWidget)
{
	ui->setupUi(this);

	createActions();

	//qff::Application *app = qff::Application::instance();
	//qff::MainWindow *fw = app->frameWork();

	{
		ui->tblPrintJobsTB->setTableView(ui->tblPrintJobs);

		ui->tblPrintJobs->setPersistentSettingsId("tblPrintJobs");
		//ui->tblPrintJobs->setRowEditorMode(qfw::TableView::EditRowsMixed);
		//ui->tblPrintJobs->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
		ui->tblPrintJobs->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblPrintJobs));
		auto m = new quickevent::og::SqlTableModel(this);
		/*
		m->addColumn("cards.id", "ID").setReadOnly(true);
		m->addColumn("cards.siId", tr("SI")).setReadOnly(true);
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("competitorName", tr("Name"));
		m->addColumn("competitors.registration", tr("Reg"));
		m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
		status_props.setGroupName("runs.status");
		m->addColumn("runs.status", tr("Status"))
				.setCastType(qMetaTypeId<qf::core::sql::DbEnum>())
				.setCastProperties(status_props);
		*/
		ui->tblPrintJobs->setTableModel(m);
		m_printJobsModel = m;
	}
}

ReceipesWidget::~ReceipesWidget()
{
	delete ui;
}

void ReceipesWidget::settleDownInPartWidget(ReceipesPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);

	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(" Check type ");
		main_tb->addWidget(lbl);
		auto *card_reader_plugin = qobject_cast<Receipes::ReceipesPlugin*>(part_widget->plugin(qf::core::Exception::Throw));
		m_cbxCardCheckers = new QComboBox();
		for(auto checker : card_reader_plugin->cardCheckers()) {
			m_cbxCardCheckers->addItem(checker->caption());
		}
		main_tb->addWidget(m_cbxCardCheckers);
	}
	*/
}

void ReceipesWidget::reload()
{
	int current_stage = currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId")
			.select2("runs", "startTimeMs, timeMs, status")
			.select2("competitors", "registration")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("cards")
			.join("cards.runId", "runs.id")
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.where("cards.stageId=" QF_IARG(current_stage))
			.orderBy("cards.id DESC");
	m_printJobsModel->setQueryBuilder(qb);
	m_printJobsModel->reload();
}

void ReceipesWidget::createActions()
{
	//QStyle *sty = style();
	/*
	{
		QIcon ico(":/quickevent/Receipes/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(onCommOpen(bool)));
		m_actCommOpen = a;
	}
	*/
}

int ReceipesWidget::currentStageId()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto event_plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	int ret = event_plugin->currentStageId();
	return ret;
}
