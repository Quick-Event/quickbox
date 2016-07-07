#include "reportoptionsdialog.h"
#include "runsplugin.h"
#include "../thispartwidget.h"
#include "../runswidget.h"
#include "../runstabledialogwidget.h"
#include "../eventstatisticswidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/utils/table.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/model/sqltablemodel.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfu = qf::core::utils;
namespace qfs = qf::core::sql;

namespace Runs {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}

static qf::qmlwidgets::framework::Plugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = fwk->plugin("Competitors");
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}

RunsPlugin::RunsPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &RunsPlugin::installed, this, &RunsPlugin::onInstalled, Qt::QueuedConnection);
}

RunsPlugin::~RunsPlugin()
{
	if(m_eventStatisticsDockWidget)
		m_eventStatisticsDockWidget->savePersistentSettingsRecursively();
}

const qf::core::utils::Table &RunsPlugin::runnersTable(int stage_id)
{
	if(m_runnersTableCacheStageId != stage_id) {
		qfs::QueryBuilder qb;
		qb.select2("competitors", "registration")
				.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
				.select2("runs", "id, siId")
				.select("runs.id AS runId")
				.select2("classes", "name")
				.from("competitors")
				.join("competitors.classId", "classes.id")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id), "JOIN")
				.orderBy("classes.name, lastName, firstName");
		qf::core::model::SqlTableModel m;
		m.setQueryBuilder(qb);
		m.reload();
		m_runnersTableCache = m.table();

		auto c_nsk = QStringLiteral("competitorNameAscii7");
		m_runnersTableCache.appendColumn(c_nsk, QVariant::String);
		int ix_nsk = m_runnersTableCache.fields().fieldIndex(c_nsk);
		int ix_cname = m_runnersTableCache.fields().fieldIndex(QStringLiteral("competitorName"));
		for (int i = 0; i < m_runnersTableCache.rowCount(); ++i) {
			qf::core::utils::TableRow &row_ref = m_runnersTableCache.rowRef(i);
			QString nsk = row_ref.value(ix_cname).toString();
			nsk = QString::fromLatin1(qf::core::Collator::toAscii7(QLocale::Czech, nsk, true));
			row_ref.setValue(ix_nsk, nsk);
		}
		m_runnersTableCacheStageId = stage_id;
	}
	return m_runnersTableCache;
}

void RunsPlugin::clearRunnersTableCache()
{
	//qfInfo() << QF_FUNC_NAME;
	m_runnersTableCacheStageId = 0;
}

void RunsPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto *tpw = new ThisPartWidget();
	m_partWidget = tpw;
	connect(tpw, &ThisPartWidget::selectedStageIdChanged, [this](int stage_id) {
		//qfInfo() << stage_id;
		this->setSelectedStageId(stage_id);
	});
	connect(competitorsPlugin(), SIGNAL(competitorEdited()), this, SLOT(clearRunnersTableCache()));

	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	{
		m_eventStatisticsDockWidget = new qff::DockWidget(nullptr);
		m_eventStatisticsDockWidget->setObjectName("eventStatisticsDockWidget");
		m_eventStatisticsDockWidget->setWindowTitle(tr("Event statistics"));		
		auto *ew = new EventStatisticsWidget();
		m_eventStatisticsDockWidget->setWidget(ew);
		fwk->addDockWidget(Qt::RightDockWidgetArea, m_eventStatisticsDockWidget);
		m_eventStatisticsDockWidget->hide();

		connect(m_eventStatisticsDockWidget, &qff::DockWidget::visibilityChanged, ew, &EventStatisticsWidget::onVisibleChanged);

		auto *a = m_eventStatisticsDockWidget->toggleViewAction();
		//a->setCheckable(true);
		a->setShortcut(QKeySequence("ctrl+shift+E"));
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
	}

	emit nativeInstalled();
}

/*
void RunsPlugin::onEditStartListRequest(int stage_id, int class_id, int competitor_id)
{
	//qf::qmlwidgets::dialogs::MessageBox::showError(nullptr, "Not implemented yet.");
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	if(!fwk->setActivePart("Runs"))
		return;
	auto *rw = partWidget()->findChild<RunsWidget*>();
	if(!rw)
		return;
	eventPlugin()->setCurrentStageId(stage_id);
	rw->editStartList(class_id, competitor_id);
}
*/
int RunsPlugin::courseForRun(int run_id)
{
	// TODO: implementation should be dependend on event type and exposed to QML
	// see: CardCheck pattern
	return courseForRun_Classic(run_id);
}

int RunsPlugin::courseForRun_Classic(int run_id)
{
	qfs::QueryBuilder qb;
	qb.select("classdefs.courseId")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.joinRestricted("competitors.classId", "classdefs.classId", "classdefs.stageId=runs.stageId")
			.where("runs.id=" QF_IARG(run_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	int cnt = 0;
	int ret = 0;
	while (q.next()) {
		if(cnt > 0) {
			qfError() << "more courses found for run_id:" << run_id;
			return 0;
		}
		ret = q.value(0).toInt();
		cnt++;
	}
	return ret;
}

void RunsPlugin::showRunsTable(int stage_id, int class_id, bool show_offrace, const QString &sort_column, int select_competitor_id)
{
	auto *w = new RunsTableDialogWidget();
	w->reload(stage_id, class_id, show_offrace, sort_column, select_competitor_id);
	qf::qmlwidgets::dialogs::Dialog dlg(this->m_partWidget);
	dlg.setButtons(QDialogButtonBox::Cancel);
	dlg.setCentralWidget(w);
	dlg.exec();
}

QWidget* RunsPlugin::createReportOptionsDialog(QWidget *parent)
{
	if(!parent) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		parent = fwk;
	}
	return new Runs::ReportOptionsDialog(parent);
}

int RunsPlugin::cardForRun(int run_id)
{
	qfLogFuncFrame() << "run id:" << run_id;
	//QF_TIME_SCOPE("reloadTimesFromCard()");
	if(!run_id)
		return 0;
	int card_id = 0;
	{
		qf::core::sql::Query q;
		if(q.exec("SELECT id FROM cards WHERE runId=" QF_IARG(run_id) " ORDER BY runIdAssignTS DESC LIMIT 1")) {
			if(q.next()) {
				card_id = q.value(0).toInt();
			}
			else {
				qfWarning() << "Cannot find card record for run id:" << run_id;
			}
		}
	}
	return card_id;
}

qf::core::utils::Table RunsPlugin::nstagesResultsTable(int stages_count, int class_id, int places)
{
	qfs::QueryBuilder qb;
	qb.select2("competitors", "id, registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("competitors")
			.where("competitors.classId=" QF_IARG(class_id));
	for (int stage_id = 1; stage_id <= stages_count; ++stage_id) {
		//qb.select("0 AS runId" QF_IARG(stage_id));
		qb.select(QF_IARG(UNREAL_TIME_MS) " AS timeMs" QF_IARG(stage_id));
		qb.select("'' AS pos" QF_IARG(stage_id));
	}
	qb.select(QF_IARG(UNREAL_TIME_MS) " AS timeMs");
	qb.select(QF_IARG(UNREAL_TIME_MS) " AS timeLossMs");
	qb.select("'' AS pos");
	qf::core::model::SqlTableModel mod;
	mod.setQueryBuilder(qb);
	mod.reload();
	QMap<int, int> competitor_id_to_row;
	for (int j = 0; j < mod.rowCount(); ++j) {
		competitor_id_to_row[mod.value(j, "competitors.id").toInt()] = j;
	}
	for (int stage_id = 1; stage_id <= stages_count; ++stage_id) {
		qfs::QueryBuilder qb;
		qb.select2("runs", "competitorId, timeMs, notCompeting, disqualified")
				.from("competitors")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id) " AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
				.where("competitors.classId=" QF_IARG(class_id))
				.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		qfs::Query q;
		q.exec(qb.toString());
		int pos = 0;
		while (q.next()) {
			++pos;
			int competitor_id = q.value("competitorId").toInt();
			int row_ix = competitor_id_to_row.value(competitor_id, -1);
			QF_ASSERT(row_ix >= 0, "Bad row index!", continue);
			QString p = QString::number(pos);
			if(q.value("notCompeting").toBool())
				p = "N";
			if(q.value("disqualified").toBool())
				p = "D";
			mod.setValue(row_ix, QString("pos%1").arg(stage_id), p);
			mod.setValue(row_ix, QString("timeMs%1").arg(stage_id), q.value("timeMs"));
		}
	}
	for (int j = 0; j < mod.rowCount(); ++j) {
		int time_ms = 0;
		for (int stage_id = 1; stage_id <= stages_count; ++stage_id) {
			QString pos_str = mod.value(j, QString("pos%1").arg(stage_id)).toString();
			int pos = pos_str.toInt();
			int tms = mod.value(j, QString("timeMs%1").arg(stage_id)).toInt();
			if(pos > 0 && tms < UNREAL_TIME_MS && time_ms < UNREAL_TIME_MS)
				time_ms += tms;
			else
				time_ms = UNREAL_TIME_MS;
		}
		mod.setValue(j, "timeMs", time_ms);
	}
	qfu::Table t = mod.table();
	t.sort("timeMs");
	int pos = 0;
	int time_ms1 = 0;
	const bool trim_disq = false;
	int trim_at = -1;
	for (int j = 0; j < t.rowCount(); ++j) {
		++pos;
		QString p = QString::number(pos) + '.';
		int time_ms = t.row(j).value("timeMs").toInt();
		int loss_ms = UNREAL_TIME_MS;
		if(time_ms < UNREAL_TIME_MS) {
			if(time_ms1 == 0)
				time_ms1 = time_ms;
			loss_ms = time_ms - time_ms1;
		}
		else {
			if(trim_disq) {
				trim_at = j;
				break;
			}
			p = "";
		}
		t.rowRef(j).setValue("pos", p);
		t.rowRef(j).setValue("timeLossMs", loss_ms);
	}
	if(trim_at < 0) {
		if(places > 0)
			trim_at = places;
	}
	else {
		if(places > 0 && trim_at > places)
			trim_at = places;
	}
	if(trim_at >= 0)
		while(t.rowCount() > trim_at)
			t.removeRow(trim_at);
	return t;
}

QVariant RunsPlugin::nstagesResultsTableData(int stages_count, int places)
{
	qfLogFuncFrame();
	//qf::core::utils::Table::FieldList cols;
	//cols << qf::core::utils::Table::Field("")
	qf::core::model::SqlTableModel mod;
	{
		qfs::QueryBuilder qb;
		qb.select2("classes", "id, name")
				.from("classes")
				//.where("name NOT IN ('D21B', 'H40B', 'H35C', 'H55B')")
				.orderBy("name");//.limit(1);
		mod.setQueryBuilder(qb);
	}
	mod.reload();
	qf::core::utils::TreeTable tt = mod.toTreeTable();
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		qfInfo() << "Processing class:" << tt_row.value("name").toString();
		int class_id = tt_row.value("id").toInt();
		qf::core::utils::Table t = nstagesResultsTable(stages_count, class_id, places);
		qf::core::utils::TreeTable tt2 = t.toTreeTable();
		tt_row.appendTable(tt2);
	}
	return tt.toVariant();
}

QVariant RunsPlugin::currentStageResultsTableData(const QString &class_filter, int max_competitors_in_class)
{
	//var event_plugin = FrameWork.plugin("Event");
	qf::core::model::SqlTableModel model;
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "id, name")
			.select2("courses", "length, climb")
			.from("classes")
			//.where("classes.name NOT IN ('D21B', 'H40B', 'H35C', 'H55B')")
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.orderBy("classes.name");//.limit(1);
		if(!class_filter.isEmpty()) {
			qb.where(class_filter);
		}
		model.setQueryBuilder(qb);
	}

	int stage_id = selectedStageId();
	{
		QVariantMap qm;
		qm["stage_id"] = stage_id;
		model.setQueryParameters(qm);
	}

	//console.info("currentStageTable query:", reportModel.effectiveQuery());
	model.reload();
	qf::core::utils::TreeTable tt = model.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", eventPlugin()->eventConfig()->value("event"));

	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "registration, lastName, firstName")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2("runs", "*")
			.select2("clubs", "name")
			.from("competitors")
			.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
			//.join("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		if(max_competitors_in_class > 0)
			qb.limit(max_competitors_in_class);
		model.setQueryBuilder(qb);
	}
	for(int i=0; i<tt.rowCount(); i++) {
		int class_id = tt.row(i).value("classes.id").toInt();
		//console.debug("class id:", class_id);
		QVariantMap qm;
		qm["stage_id"] = stage_id;
		qm["class_id"] = class_id;
		model.setQueryParameters(qm);
		model.reload();
		qf::core::utils::TreeTable tt2 = model.toTreeTable();
		tt2.appendColumn("pos", QVariant::Int);
		for(int j=0; j<tt2.rowCount(); j++) {
			qf::core::utils::TreeTableRow row = tt2.row(j);
			bool has_pos = !row.value(QStringLiteral("disqualified")).toBool() && !row.value(QStringLiteral("notCompeting")).toBool();
			if(has_pos)
				row.setValue("pos", j+1);
			else
				row.setValue("pos", 0);
		}
		tt.row(i).appendTable(tt2);
	}
	//console.debug(tt.toString());
	return tt.toVariant();
}

}
