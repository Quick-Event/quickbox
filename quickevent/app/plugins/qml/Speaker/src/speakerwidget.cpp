#include "speakerwidget.h"
#include "ui_speakerwidget.h"

#include "codeclassresultswidget.h"
#include "thispartwidget.h"

#include "Speaker/speakerplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/core/si/punchrecord.h>
#include <quickevent/core/si/siid.h>
#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/core/og/timems.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>

#include <QDockWidget>
#include <QJsonObject>
#include <QLabel>
#include <QSettings>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

SpeakerWidget::SpeakerWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::SpeakerWidget)
{
	ui->setupUi(this);

	ui->tblPunchesToolBar->setTableView(ui->tblPunches);

	ui->tblPunches->setReadOnly(true);
	ui->tblPunches->setCloneRowEnabled(false);
	ui->tblPunches->setPersistentSettingsId("tblPunches");

	connect(ui->tblPunches, &PunchesTableView::codeClassActivated, this, &SpeakerWidget::onCodeClassActivated);
	/*
	ui->tblPunches->setDragEnabled(true);
	//ui->tblView->setDragDropMode(QAbstractItemView::DragOnly);
	ui->tblPunches->setDragDropMode(QAbstractItemView::InternalMove);
	ui->tblPunches->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tblPunches->viewport()->setAcceptDrops(true);
	ui->tblPunches->setDropIndicatorShown(true);
	*/
	//ui->tblPunches->setRowEditorMode(qfw::TableView::EditRowsMixed);
	//ui->tblPunches->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	quickevent::core::og::SqlTableModel *m = new quickevent::core::og::SqlTableModel(this);
	m->addColumn("punches.id");//.setReadOnly(true);
	m->addColumn("punches.code", tr("Code"));
	m->addColumn("punches.siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::core::si::SiId>());
	m->addColumn("punches.timeMs", tr("Punch time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>());
	m->addColumn("punches.runTimeMs", tr("Runner time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>());
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitors.registration", tr("Registration"));
	m->addColumn("competitorName", tr("Competitor"));
	ui->tblPunches->setTableModel(m);
	m_punchesModel = m;

	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	connect(fwk, &qf::qmlwidgets::framework::MainWindow::aboutToClose, this, &SpeakerWidget::saveSettings);
}

SpeakerWidget::~SpeakerWidget()
{
	delete ui;
}

void SpeakerWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	m_partWidget = part_widget;

	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));

	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &SpeakerWidget::onDbEventNotify, Qt::QueuedConnection);
	connect(eventPlugin(), &Event::EventPlugin::eventOpened, this, &SpeakerWidget::loadSettings, Qt::QueuedConnection);
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);
	*/
	//qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	/*
	QLabel *lbl_class;
	{
		lbl_class = new QLabel(tr("&Class "));
		main_tb->addWidget(lbl_class);
	}
	{
		m_cbxClasses = new qfw::ForeignKeyComboBox();
		m_cbxClasses->setMinimumWidth(fontMetrics().width('X') * 10);
		m_cbxClasses->setMaxVisibleItems(100);
		m_cbxClasses->setReferencedTable("classes");
		m_cbxClasses->setReferencedField("id");
		m_cbxClasses->setReferencedCaptionField("name");
		main_tb->addWidget(m_cbxClasses);
	}
	lbl_class->setBuddy(m_cbxClasses);
	*/
}

void SpeakerWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	if(!isPartActive())
		return;
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED)) {
		quickevent::core::si::PunchRecord punch(data.toMap());
		int siid = punch.siid();
		if(siid > 0 && punch.marking() == quickevent::core::si::PunchRecord::MARKING_RACE) {
			updateTableView(punch.id());
			emit punchReceived(punch);
		}
	}
}

void SpeakerWidget::reset()
{
	qfInfo() << Q_FUNC_INFO;
	if(!eventPlugin()->isEventOpen()) {
		m_punchesModel->clearRows();
		return;
	}
	ui->classResults->reset(0, 0, CodeClassResultsWidget::RESULTS_PUNCH_CODE);
	if(isPartActive())
		reload();
	else
		m_resetRequest = true;
}

void SpeakerWidget::reload()
{
	qfInfo() << Q_FUNC_INFO << isPartActive();
	if(!isPartActive())
		return;
	if(m_resetRequest) {
		m_resetRequest = false;
		reset();
		return;
	}
	int stage_id = eventPlugin()->currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("punches", "*")
			.select2("classes", "id, name")
			.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("punches")
			.join("punches.runId", "runs.id")
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.where("punches.stageId=" QF_IARG(stage_id))
			.orderBy("punches.id DESC");//.limit(10);
	//qfWarning() << qb.toString();
	m_punchesModel->setQueryBuilder(qb, false);
	m_punchesModel->reload();
}

void SpeakerWidget::updateTableView(int punch_id)
{
	if(punch_id <= 0)
		return;
	m_punchesModel->insertRow(0);
	m_punchesModel->setValue(0, QStringLiteral("punches.id"), punch_id);
	int reloaded_row_cnt = m_punchesModel->reloadRow(0);
	if(reloaded_row_cnt != 1) {
		qfWarning() << "Inserted/Copied row id:" << punch_id << "reloaded in" << reloaded_row_cnt << "instances.";
		return;
	}
	ui->tblPunches->updateRow(0);
}

void SpeakerWidget::loadSettings()
{
}

void SpeakerWidget::saveSettings()
{
}

bool SpeakerWidget::isPartActive()
{
	return m_partWidget && m_partWidget->isActive();
}

void SpeakerWidget::onCodeClassActivated(int class_id, int code)
{
	CodeClassResultsWidget *w = new CodeClassResultsWidget(this);
	w->reset(class_id, code);
	//if(eventPlugin()->isEventOpen())
	//	w->loadSetup(QJsonObject());
	connect(this, &SpeakerWidget::punchReceived, w, &CodeClassResultsWidget::onPunchReceived);

	QDockWidget *dw = new QDockWidget();
	static int dock_widget_no = 0;
	dw->setObjectName("CodeClassResultsWidgetDockWidget_" + QString::number(++dock_widget_no));
	dw->setAllowedAreas(Qt::AllDockWidgetAreas);
	dw->setWidget(w);
	//dw->show();
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	fwk->addDockWidget(Qt::LeftDockWidgetArea, dw);
	dw->setFloating(true);
}

