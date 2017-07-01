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

#include <QDropEvent>
#include <QMimeData>
#include <QJsonObject>
#include <QTimer>

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

CodeClassResultsWidget::CodeClassResultsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CodeClassResultsWidget)
{
	ui->setupUi(this);
	{
		qf::core::sql::Query q;
		q.exec("SELECT id, name FROM classes ORDER BY name", qf::core::Exception::Throw);
		while(q.next()) {
			ui->lstClass->addItem(q.value(1).toString(), q.value(0));
		}
	}
	connect(ui->lstClass, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int ) {
		int stage_id = eventPlugin()->currentStageId();
		int class_id = this->ui->lstClass->currentData().toInt();
		ui->lstCode->clear();
		ui->lstCode->addItem(tr("Results"), "R");
		ui->lstCode->addItem(tr("Finish"), quickevent::si::PunchRecord::FINISH_PUNCH_CODE);
		qf::core::sql::QueryBuilder qb;
		qb.select2("codes", "code, radio")
				.from("classdefs")
				.joinRestricted("classdefs.courseId", "coursecodes.courseId",
								"classdefs.stageId=" QF_IARG(stage_id)
								" AND classdefs.classId=" QF_IARG(class_id),
								qf::core::sql::QueryBuilder::INNER_JOIN)
				.join("coursecodes.codeId", "codes.id", qf::core::sql::QueryBuilder::INNER_JOIN)
				//.joinRestricted("coursecodes.codeId", "codes.id", "codes.radio", qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("coursecodes.position");
		qfInfo() << qb.toString();
		qf::core::sql::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while(q.next()) {
			QVariant code = q.value(0);
			bool radio =  q.value(1).toBool();
			QString caption = code.toString();
			if(radio)
				caption += " " + tr("R", "Radio station");
			ui->lstCode->addItem(caption, code);
		}
	});

	//connect(ui->btReload, &QPushButton::clicked, this, &CodeClassResultsWidget::reload);
	connect(ui->lstCode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CodeClassResultsWidget::reloadDeferred);

	ui->tblView->setReadOnly(true);
	//ui->tblView->setPersistentSettingsId("tblView");
	//ui->tblPunches->setRowEditorMode(qfw::TableView::EditRowsMixed);
	//ui->tblPunches->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	quickevent::og::SqlTableModel *m = new quickevent::og::SqlTableModel(this);
	m->addColumn("competitors.registration", tr("Reg"));//.setReadOnly(true);
	m->addColumn("competitorName", tr("Competitor"));
	m->addColumn("timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	ui->tblView->setTableModel(m);
	m_tableModel = m;
}

CodeClassResultsWidget::~CodeClassResultsWidget()
{
	delete ui;
}

void CodeClassResultsWidget::reloadDeferred()
{
	if(!m_reloadDeferredTimer) {
		m_reloadDeferredTimer = new QTimer(this);
		m_reloadDeferredTimer->setSingleShot(true);
		m_reloadDeferredTimer->setInterval(200);
		connect(m_reloadDeferredTimer, &QTimer::timeout, this, &CodeClassResultsWidget::reload);
	}
	if(m_reloadDeferredTimer->isActive())
		return;
	m_reloadDeferredTimer->start();
}

void CodeClassResultsWidget::reload()
{
	if(!eventPlugin()->isEventOpen())
		return;
	int stage_id = eventPlugin()->currentStageId();
	int class_id = this->ui->lstClass->currentData().toInt();
	int code = this->ui->lstCode->currentData().toInt();
	qf::core::sql::QueryBuilder qb;
	qb.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName");
	if(code == 0) {
		// results
		qb.select2("runs", "timeMs")
				.from("runs")
				.where("runs.stageId=" QF_IARG(stage_id))
				.joinRestricted("runs.competitorId", "competitors.id",
								"competitors.classId=" QF_IARG(class_id)
								" AND NOT runs.disqualified"
								" AND timeMs IS NOT NULL" ,
								qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("runs.timeMs");//.limit(10);
	}
	/*
	else if(code == quickevent::si::PunchRecord::FINISH_PUNCH_CODE) {
		qb.select2("runlaps", "stpTimeMs AS timeMs")
				.from("runlaps")
				.joinRestricted("runlaps.runId", "runs.id",
								"runs.stageId=" QF_IARG(stage_id)
								" AND runlaps.code=" QF_IARG(code)
								" AND NOT runs.disqualified",
								qf::core::sql::QueryBuilder::INNER_JOIN)
				.joinRestricted("runs.competitorId", "competitors.id", "competitors.classId=" QF_IARG(class_id), qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("runlaps.stpTimeMs");//.limit(10);
	}
	*/
	else {
		qb.select2("punches", "runTimeMs AS timeMs")
				.from("punches")
				.joinRestricted("punches.runId", "runs.id",
								"punches.stageId=" QF_IARG(stage_id)
								" AND punches.code=" QF_IARG(code)
								" AND NOT runs.disqualified",
								qf::core::sql::QueryBuilder::INNER_JOIN)
				.joinRestricted("runs.competitorId", "competitors.id", "competitors.classId=" QF_IARG(class_id), qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("punches.runTimeMs");//.limit(10);
	}
	qfInfo() << qb.toString();
	m_tableModel->setQueryBuilder(qb, false);
	m_tableModel->reload();
}

void CodeClassResultsWidget::onPunchReceived(const quickevent::si::PunchRecord &punch)
{
	int code = this->ui->lstCode->currentData().toInt();
	if(code > 0 && punch.code() == code) {
		reload();
	}
}

void CodeClassResultsWidget::reset(int class_id, int code)
{
	ui->lstClass->setCurrentIndex(ui->lstClass->findData(class_id));
	ui->lstCode->setCurrentIndex(ui->lstCode->findData(code));
}

void CodeClassResultsWidget::loadSetup(const QJsonObject &jso)
{
	reset(jso.value("classId").toInt(), jso.value("codeId").toInt());
	/*
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
	*/
}

QJsonObject CodeClassResultsWidget::saveSetup()
{
	QJsonObject ret;
	ret["classId"] = ui->lstClass->currentData().toInt();
	ret["codeId"] = ui->lstCode->currentData().toInt();
	return ret;
}
#if 0
void CodeClassResultsWidget::dropEvent(QDropEvent *event)
{
	qfInfo() << event->mimeData()->hasText();
	Super::dropEvent(event);
	/*
	if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
		QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QPixmap pixmap;
		QPoint offset;
		dataStream >> pixmap >> offset;

		QLabel *newIcon = new QLabel(this);
		newIcon->setPixmap(pixmap);
		newIcon->move(event->pos() - offset);
		newIcon->show();
		newIcon->setAttribute(Qt::WA_DeleteOnClose);

		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
	*/
}
#endif
