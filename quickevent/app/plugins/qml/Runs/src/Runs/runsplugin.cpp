#include "runsplugin.h"
#include "nstagesreportoptionsdialog.h"
#include "../thispartwidget.h"
#include "../runswidget.h"
#include "../runstabledialogwidget.h"
#include "../eventstatisticswidget.h"
#include "../printawardsoptionsdialogwidget.h"
#include "../services/resultsexporter.h"

#include <Event/eventplugin.h>

#include <quickevent/gui/reportoptionsdialog.h>

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
#include <qf/core/utils/htmlutils.h>
#include <qf/core/utils/table.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/model/sqltablemodel.h>

#include <QFile>
#include <QInputDialog>
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
		m.setQueryBuilder(qb, false);
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
		m_eventStatisticsDockWidget->setPersistentSettingsId("Runs/eventStatistics");
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

	services::ResultsExporter *results_exporter = new services::ResultsExporter(this);
	services::Service::addService(results_exporter);

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
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	if(is_relays) {
		return courseForRun_Relays(run_id);
	}
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

int RunsPlugin::courseForRun_Relays(int run_id)
{
	int ret = 0;
	qfs::QueryBuilder qb;
	qb.select2("relays", "number")
			.select2("runs", "leg")
			.from("runs")
			.join("runs.relayId", "relays.id")
			.where("runs.id=" QF_IARG(run_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	if(q.next()) {
		QString relay_num = q.value("number").toString();
		QString leg = q.value("leg").toString();
		q.exec("SELECT id FROM courses WHERE name='" + relay_num + '.' + leg + "'");
		int cnt = 0;
		while (q.next()) {
			if(cnt > 0) {
				qfError() << "more courses found for run_id:" << run_id;
				return 0;
			}
			ret = q.value(0).toInt();
			cnt++;
		}
	}
	else {
		qfError() << "Cannot find relays record for run id:" << run_id;
	}
	return ret;
}

void RunsPlugin::showRunsTable(int stage_id, int class_id, bool show_offrace, const QString &sort_column, int select_competitor_id)
{
	auto *w = new RunsTableDialogWidget();
	w->reload(stage_id, class_id, show_offrace, sort_column, select_competitor_id);
	qf::qmlwidgets::dialogs::Dialog dlg(this->m_partWidget);
	//dlg.setButtons(QDialogButtonBox::Cancel);
	dlg.setCentralWidget(w);
	dlg.exec();
}

QWidget* RunsPlugin::createReportOptionsDialog(QWidget *parent)
{
	if(!parent) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		parent = fwk;
	}
	quickevent::gui::ReportOptionsDialog *ret = new quickevent::gui::ReportOptionsDialog(parent);
	ret->loadPersistentSettings();
	return ret;
}

QWidget *RunsPlugin::createNStagesReportOptionsDialog(QWidget *parent)
{
	if(!parent) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		parent = fwk;
	}
	return new Runs::NStagesReportOptionsDialog(parent);
}

bool RunsPlugin::reloadTimesFromCard(int run_id)
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *cardreader_plugin = fwk->plugin("CardReader");
	if(!cardreader_plugin) {
		qfError() << "CardReader plugin not installed!";
		return false;
	}
	int card_id = cardForRun(run_id);
	bool ok;
	QMetaObject::invokeMethod(cardreader_plugin, "reloadTimesFromCard", Qt::DirectConnection,
							  Q_RETURN_ARG(bool, ok),
							  Q_ARG(int, card_id),
							  Q_ARG(int, run_id));
	return ok;
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

qf::core::utils::Table RunsPlugin::nstagesResultsTable(int stages_count, int class_id, int places, bool exclude_disq)
{
	qfs::QueryBuilder qb;
	qb.select2("competitors", "id, registration, licence")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("competitors")
			.where("competitors.classId=" QF_IARG(class_id));
	for (int stage_id = 1; stage_id <= stages_count; ++stage_id) {
		//qb.select("0 AS runId" QF_IARG(stage_id));
		qb.select(QF_IARG(UNREAL_TIME_MSEC) " AS timeMs" QF_IARG(stage_id));
		qb.select("'' AS pos" QF_IARG(stage_id));
	}
	qb.select(QF_IARG(UNREAL_TIME_MSEC) " AS timeMs");
	qb.select(QF_IARG(UNREAL_TIME_MSEC) " AS timeLossMs");
	qb.select("'' AS pos");
	qf::core::model::SqlTableModel mod;
	mod.setQueryBuilder(qb, false);
	mod.reload();
	QMap<int, int> competitor_id_to_row;
	for (int j = 0; j < mod.rowCount(); ++j) {
		competitor_id_to_row[mod.value(j, "competitors.id").toInt()] = j;
	}
	for (int stage_id = 1; stage_id <= stages_count; ++stage_id) {
		qfs::QueryBuilder qb;
		qb.select2("runs", "competitorId, timeMs, notCompeting, disqualified")
				.from("competitors")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id) " AND runs.isRunning AND runs.finishTimeMs>0", "JOIN")
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
			if(pos > 0 && tms < UNREAL_TIME_MSEC && time_ms < UNREAL_TIME_MSEC)
				time_ms += tms;
			else
				time_ms = UNREAL_TIME_MSEC;
		}
		mod.setValue(j, "timeMs", time_ms);
	}
	qfu::Table t = mod.table();
	t.sort("timeMs");
	int pos = 0;
	int time_ms1 = 0;
	//const bool trim_disq = false;
	int trim_at = -1;
	for (int j = 0; j < t.rowCount(); ++j) {
		++pos;
		QString p = QString::number(pos) + '.';
		int time_ms = t.row(j).value("timeMs").toInt();
		int loss_ms = UNREAL_TIME_MSEC;
		if(time_ms < UNREAL_TIME_MSEC) {
			if(time_ms1 == 0)
				time_ms1 = time_ms;
			loss_ms = time_ms - time_ms1;
		}
		else {
			if(exclude_disq) {
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

QVariant RunsPlugin::nstagesResultsTableData(int stages_count, int places, bool exclude_disq)
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
		mod.setQueryBuilder(qb, false);
	}
	mod.reload();
	qf::core::utils::TreeTable tt = mod.toTreeTable();
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		qfInfo() << "Processing class:" << tt_row.value("name").toString();
		int class_id = tt_row.value("id").toInt();
		qf::core::utils::Table t = nstagesResultsTable(stages_count, class_id, places, exclude_disq);
		qf::core::utils::TreeTable tt2 = t.toTreeTable();
		tt_row.appendTable(tt2);
	}
	return tt.toVariant();
}

QVariant RunsPlugin::currentStageResultsTableData(const QString &class_filter, int max_competitors_in_class, bool exclude_disq)
{
	int stage_id = selectedStageId();
	return stageResultsTableData(stage_id, class_filter, max_competitors_in_class, exclude_disq);
}

qf::core::utils::TreeTable RunsPlugin::currentStageResultsTable(const QString &class_filter, int max_competitors_in_class, bool exclude_disq)
{
	int stage_id = selectedStageId();
	return stageResultsTable(stage_id, class_filter, max_competitors_in_class, exclude_disq);
}

qf::core::utils::TreeTable RunsPlugin::stageResultsTable(int stage_id, const QString &class_filter, int max_competitors_in_class, bool exclude_disq)
{
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
		model.setQueryBuilder(qb, true);
	}
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
	tt.setValue("stageStart", eventPlugin()->stageStartDateTime(stage_id));

	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "registration, lastName, firstName")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2("runs", "*")
			.select2("clubs", "name")
			.from("competitors")
			.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
			.joinRestricted("competitors.id"
							, "runs.competitorId"
							, QStringLiteral("runs.stageId={{stage_id}}"
											 " AND runs.isRunning"
											 " AND (runs.finishTimeMs>0 OR runs.checkTimeMs IS NOT NULL)")
							+ (exclude_disq? " AND NOT runs.disqualified": "")
							, "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		if(max_competitors_in_class > 0)
			qb.limit(max_competitors_in_class);
		model.setQueryBuilder(qb, true);
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
		tt2.appendColumn("pos", QVariant::String);
		tt2.appendColumn("npos", QVariant::Int);
		int prev_time_ms = 0;
		int prev_pos = 0;
		for(int j=0; j<tt2.rowCount(); j++) {
			qf::core::utils::TreeTableRow row = tt2.row(j);
			bool has_pos = !row.value(QStringLiteral("disqualified")).toBool() && !row.value(QStringLiteral("notCompeting")).toBool();
			int time_ms = row.value(QStringLiteral("timeMs")).toInt();
			if(has_pos) {
				int pos = j+1;
				if(time_ms == prev_time_ms)
					pos = prev_pos;
				else
					prev_pos = pos;
				row.setValue("pos", QString::number(pos) + '.');
				row.setValue("npos", pos);
			}
			else {
				row.setValue("pos", QString());
				row.setValue("npos", 0);
			}
			prev_time_ms = time_ms;
		}
		tt.row(i).appendTable(tt2);
	}
	return tt;
}

QVariant RunsPlugin::stageResultsTableData(int stage_id, const QString &class_filter, int max_competitors_in_class, bool exclude_disq)
{
	qf::core::utils::TreeTable tt = stageResultsTable(stage_id, class_filter, max_competitors_in_class, exclude_disq);
	return tt.toVariant();
}

QVariantMap RunsPlugin::printAwardsOptionsWithDialog(const QVariantMap &opts)
{
	qfInfo() << Q_FUNC_INFO;
	QVariantMap ret;
	PrintAwardsOptionsDialogWidget *w = new PrintAwardsOptionsDialogWidget();
	w->setPrintOptions(opts);
	qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, partWidget());
	dlg.setCentralWidget(w);
	QString plugin_home = manifest()->homeDir();
	w->init(plugin_home);
	if(dlg.exec()) {
		ret = w->printOptions();
	}
	return ret;
}

bool RunsPlugin::exportResultsIofXml30Stage(int stage_id, const QString &file_name)
{
	QFile f(file_name);
	if(!f.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << f.fileName() << "for writing.";
		return false;
	}

	//int start00_msec = eventPlugin()->stageStartMsec(stage_id);
	QDateTime stage_start_date_time = eventPlugin()->stageStartDateTime(stage_id);

	qf::core::utils::TreeTable tt1 = stageResultsTable(stage_id);
	QVariantList result_list{
		"ResultList",
		QVariantMap{
			{"xmlns", "http://www.orienteering.org/datastandard/3.0"},
			{"status", "Complete"},
			{"iofVersion", "3.0"},
			{"creator", "QuickEvent"},
			{"createTime", QDateTime::currentDateTime().toString(Qt::ISODate)},
		}
	};
	{
		QVariantList event_lst{"Event"};
		QVariantMap event = tt1.value("event").toMap();
		event_lst.insert(event_lst.count(), QVariantList{"Id", QVariantMap{{"type", "ORIS"}}, event.value("importId")});
		event_lst.insert(event_lst.count(), QVariantList{"Name", event.value("name")});
		event_lst.insert(event_lst.count(), QVariantList{"StartTime",
				   QVariantList{"Date", event.value("date")},
				   QVariantList{"Time", event.value("time")}
		});
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "director"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("director").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("director").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "mainReferee"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("mainReferee").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("mainReferee").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		result_list.insert(result_list.count(), event_lst);
	}

	for(int i=0; i<tt1.rowCount(); i++) {
		QVariantList class_result{"ClassResult"};
		const qf::core::utils::TreeTableRow row1 = tt1.row(i);
		class_result.insert(class_result.count(),
			QVariantList{"Class",
				QVariantList{"Name", row1.value("classes.name") },
			}
		);
		class_result.insert(class_result.count(),
			QVariantList{"Course",
				QVariantList{"Length", row1.value("courses.length") },
				QVariantList{"Climb", row1.value("courses.climb") },
			}
		);
		qf::core::utils::TreeTable tt2 = row1.table();
		//int pos = 0;
		for(int j=0; j<tt2.rowCount(); j++) {
			const qf::core::utils::TreeTableRow row2 = tt2.row(j);
			//pos++;
			QVariantList person_result{"PersonResult"};
			person_result.insert(person_result.count(),
				 QVariantList{"Person",
					 QVariantList{"Id", QVariantMap{{"type", "CZE"}}, row2.value("competitors.registration")},
					 QVariantList{"Name",
						QVariantList{"Family", row2.value("competitors.lastName")},
						QVariantList{"Given", row2.value("competitors.firstName")},
					 }
				 }
			);

			QVariantList result{"Result"};
			int stime = row2.value("startTimeMs").toInt() / 1000;
			int ftime = row2.value("finishTimeMs").toInt() / 1000;
			int time = row2.value("timeMs").toInt() / 1000;
			if(ftime && time)
				stime = ftime - time; // cover cases when competitor didn't started according to start list from any reason
			result.insert(result.count(), QVariantList{"StartTime", stage_start_date_time.addMSecs(stime).toString(Qt::ISODate)});
			result.insert(result.count(), QVariantList{"FinishTime", stage_start_date_time.addMSecs(ftime).toString(Qt::ISODate)});
			result.insert(result.count(), QVariantList{"Time", time});

			static auto STAT_OK = QStringLiteral("OK");
			QString competitor_status = STAT_OK;
			if (!ftime) {
				 competitor_status = "DidNotFinish";
			}
			else if (row2.value("disqualified").toBool()) {
				if (row2.value("misPunch").toBool())
					competitor_status = "MissingPunch";
				else
					competitor_status = "Disqualified";
			}
			else if (row2.value("notCompeting").toBool())
				competitor_status = "Inactive";
			if (competitor_status == STAT_OK) {
				// The position in the result list for the person that the result belongs to.
				// This element should only be present when the Status element is set to OK.
				result.insert(result.count(), QVariantList{"Position", row2.value("npos")});
			}
			result.insert(result.count(), QVariantList{"Status", competitor_status});

			qfs::QueryBuilder qb;
			qb.select2("runlaps", "*")
				.from("runlaps")
				.where("runlaps.runId=" + QString::number(row2.value("runs.id").toInt()))
				.where("runlaps.code!=999") // omit finish lap
				.orderBy("runlaps.position");

			qfs::Query q;
			q.exec(qb.toString());
			while(q.next()) {
				//console.info(k, reportModel.value(k, "position"));
				result.insert(result.count(),
					QVariantList{"SplitTime",
						QVariantList{"ControlCode", q.value("code") },
						QVariantList{"Time", q.value("stpTimeMs").toInt() / 1000 },
					}
				);
			}

			person_result.insert(person_result.count(), result);
			class_result.insert(class_result.count(), person_result);
		}
		result_list.insert(result_list.count(), class_result);
	}

	qf::core::utils::HtmlUtils::FromXmlListOptions opts;
	opts.setDocumentTitle(tr("E%1 IOF XML stage results").arg(tt1.value("stageId").toString()));
	QString str = qf::core::utils::HtmlUtils::fromXmlList(result_list, opts);
	f.write(str.toUtf8());
	qfInfo() << "exported:" << file_name;
	return true;
}

static QString make_width(const QString &s, int width)
{
	static const auto SS = QStringLiteral("%1");
	int abs_width = width < 0? -width: width;
	QString ret = SS.arg(s, width, QChar(' ')).mid(0, abs_width);
	return ret;
}

void RunsPlugin::writeCSOSHeader(QTextStream &ts)
{
	Event::EventPlugin *evp = eventPlugin();
	int stage_id = selectedStageId();
	QDateTime start_dt = evp->stageStartDateTime(stage_id);
	Event::EventConfig *ec = evp->eventConfig();
	static constexpr int HWIDTH = -19;
	ts << make_width("Kod zavodu", HWIDTH) << ": " << ec->importId() << "\r\n";
	ts << make_width("Nazev zavodu", HWIDTH) << ": " << ec->eventName() << "\r\n";
	ts << make_width("Zarazeni do soutezi", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Datum konani", HWIDTH) << ": " << start_dt.toString(Qt::ISODate) << "\r\n";
	ts << make_width("Misto konani", HWIDTH) << ": " << ec->eventPlace() << "\r\n";
	ts << make_width("Poradatel", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Mapa", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Reditel zavodu", HWIDTH) << ": " << ec->director() << "\r\n";
	ts << make_width("Hlavni rozhodci", HWIDTH) << ": " << ec->mainReferee() << "\r\n";
	ts << make_width("Stavitel trati", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("JURY", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Protokol", HWIDTH) << ": " << "\r\n";
	ts << "----------------------------------------------------------------------------\r\n";
}

bool RunsPlugin::exportResultsCsosStage(int stage_id, const QString &file_name)
{
	QFile f(file_name);
	if(!f.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << f.fileName() << "for writing.";
		return false;
	}
	QTextStream ts(&f);
	ts.setCodec("CP1250");
	writeCSOSHeader(ts);

	qfs::QueryBuilder qb;
	qb.select2("competitors", "registration, lastName, firstName, licence")
		.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
		.select2("runs", "*")
		.select2("classes", "name")
		.from("competitors")
		.join("competitors.classId", "classes.id")
		.joinRestricted("competitors.id"
						, "runs.competitorId"
						, QStringLiteral("runs.stageId=%1 AND runs.isRunning AND runs.finishTimeMs>0").arg(stage_id)
						, qfs::QueryBuilder::INNER_JOIN)
		//.where("competitors.classId={{class_id}}")
		.orderBy("classes.name, runs.notCompeting, runs.disqualified, runs.timeMs");
	qfs::Query q;
	q.exec(qb.toString());
	while(q.next()) {
		ts << make_width(q.value(QStringLiteral("classes.name")).toString(), -10);
		ts << make_width(q.value(QStringLiteral("competitorName")).toString(), -25);
		ts << make_width(q.value(QStringLiteral("registration")).toString(), -7);
		ts << (q.value(QStringLiteral("runs.notCompeting")).toBool()? "M": make_width(q.value(QStringLiteral("licence")).toString(), 1));
		int ms = q.value(QStringLiteral("runs.timeMs")).toInt();
		QString time_str;
		if(q.value(QStringLiteral("runs.disqualified")).toBool())
			time_str = QStringLiteral("888.88");
		/*
		else if(PRUMER neni podporovan v QE)
			time_str = QStringLiteral("999.99");
		*/
		else
			time_str = make_width(quickevent::core::og::TimeMs(ms).toString(), 6);
		ts << time_str;
		ts << "\r\n";
	}
	return true;
}

bool RunsPlugin::exportResultsCsosOverall(int stage_count, const QString &file_name)
{
	QFile f(file_name);
	if(!f.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << f.fileName() << "for writing.";
		return false;
	}
	QTextStream ts(&f);
	ts.setCodec("CP1250");
	writeCSOSHeader(ts);

	qfs::Query q;
	q.exec("SELECT id, name FROM classes ORDER BY name");
	while(q.next()) {
		int class_id = q.value(0).toInt();
		QString class_name = q.value(1).toString();
		qf::core::utils::Table tt = nstagesResultsTable(stage_count, class_id, -1, false);
		for (int i = 0; i < tt.rowCount(); ++i) {
			qf::core::utils::TableRow row = tt.row(i);
			ts << make_width(class_name, -10);
			ts << make_width(row.value(QStringLiteral("competitorName")).toString(), -25);
			ts << make_width(row.value(QStringLiteral("registration")).toString(), -7);
			ts << (row.value(QStringLiteral("pos")).toString() == QLatin1String("N")? "M": make_width(row.value(QStringLiteral("licence")).toString(), 1));
			int ms = row.value(QStringLiteral("timeMs")).toInt();
			QString time_str;
			if(row.value(QStringLiteral("pos")).toString() == QLatin1String("D"))
				time_str = QStringLiteral("888.88");
			/*
			else if(PRUMER neni podporovan v QE)
				time_str = QStringLiteral("999.99");
			*/
			else
				time_str = make_width(quickevent::core::og::TimeMs(ms).toString(), 6);
			ts << time_str;
			ts << "\r\n";
		}
	}
	return true;
}

}
