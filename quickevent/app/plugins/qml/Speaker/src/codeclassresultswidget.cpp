#include "codeclassresultswidget.h"
#include "ui_codeclassresultswidget.h"

#include "Event/eventplugin.h"

#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/timems.h>
#include <quickevent/si/punchrecord.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QJsonObject>

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

CodeClassResultsWidget::CodeClassResultsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CodeClassResultsWidget)
{
	ui->setupUi(this);

	connect(ui->lstClass, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int ) {
		int stage_id = eventPlugin()->currentStageId();
		int class_id = this->ui->lstClass->currentData().toInt();
		ui->lstCode->clear();
		qf::core::sql::QueryBuilder qb;
		qb.select2("codes", "code")
				.from("classdefs")
				.joinRestricted("classdefs.courseId", "coursecodes.courseId",
								"classdefs.stageId=" QF_IARG(stage_id)
								" AND classdefs.classId=" QF_IARG(class_id),
								qf::core::sql::QueryBuilder::INNER_JOIN)
				.joinRestricted("coursecodes.codeId", "codes.id", "codes.radio", qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("coursecodes.position");
		//qfWarning() << qb.toString();
		qf::core::sql::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while(q.next()) {
			ui->lstCode->addItem(q.value(0).toString(), q.value(0));
		}
	});

	ui->tblView->setReadOnly(true);
	//ui->tblView->setPersistentSettingsId("tblView");
	//ui->tblPunches->setRowEditorMode(qfw::TableView::EditRowsMixed);
	//ui->tblPunches->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	quickevent::og::SqlTableModel *m = new quickevent::og::SqlTableModel(this);
	m->addColumn("competitors.registration", tr("Reg"));//.setReadOnly(true);
	m->addColumn("competitorName", tr("Competitor"));
	m->addColumn("punches.runTimeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	ui->tblView->setTableModel(m);
	m_tableModel = m;
}

CodeClassResultsWidget::~CodeClassResultsWidget()
{
	delete ui;
}

void CodeClassResultsWidget::reload()
{
	int stage_id = eventPlugin()->currentStageId();
	int class_id = this->ui->lstClass->currentData().toInt();
	int code = this->ui->lstCode->currentData().toInt();
	qf::core::sql::QueryBuilder qb;
	qb.select2("punches", "runTimeMs")
			.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("punches")
			.joinRestricted("punches.runId", "runs.id",
							"punches.stageId=" QF_IARG(stage_id)
							" AND punches.code=" QF_IARG(code)
							" AND NOT runs.disqualified")
			.joinRestricted("runs.competitorId", "competitors.id", "competitors.classId=" QF_IARG(class_id))
			.orderBy("punches.runTimeMs");//.limit(10);
}

void CodeClassResultsWidget::onPunchReceived(const quickevent::si::PunchRecord &punch)
{
	int code = this->ui->lstCode->currentData().toInt();
	if(code > 0 && punch.code() == code) {
		reload();
	}
}

void CodeClassResultsWidget::loadSetup(const QJsonObject &jso)
{
	ui->lstClass->blockSignals(true);
	ui->lstClass->clear();
	qf::core::sql::Query q;
	q.exec("SELECT id, name FROM classes ORDER BY name", qf::core::Exception::Throw);
	while(q.next()) {
		ui->lstClass->addItem(q.value(1).toString(), q.value(0));
	}
	ui->lstClass->blockSignals(false);
	ui->lstClass->setCurrentIndex(ui->lstClass->findData(jso.value("classId").toInt()));
	ui->lstCode->setCurrentIndex(ui->lstCode->findData(jso.value("codeId").toInt()));
}

QJsonObject CodeClassResultsWidget::saveSetup()
{
	QJsonObject ret;
	ret["classId"] = ui->lstClass->currentData().toInt();
	ret["codeId"] = ui->lstCode->currentData().toInt();
	return ret;
}
