#include "runsplugin.h"
#include "nstagesreportoptionsdialog.h"
#include "thispartwidget.h"
#include "runswidget.h"
#include "runstabledialogwidget.h"
#include "eventstatisticswidget.h"
#include "printawardsoptionsdialogwidget.h"
#include "services/resultsexporter.h"

#include <Event/eventplugin.h>

#include <quickevent/gui/reportoptionsdialog.h>
#include <quickevent/core/si/codedef.h>
#include <quickevent/core/si/punchrecord.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/utils/table.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/model/sqltablemodel.h>

#include <QDesktopServices>
#include <QFile>
#include <QInputDialog>
#include <QDir>
#include <QUrl>

#include <math.h>

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
	: Super("Runs", parent)
{
	connect(this, &RunsPlugin::installed, this, &RunsPlugin::onInstalled);
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
	Event::services::Service::addService(results_exporter);

	//emit nativeInstalled();
	auto *a_print = m_partWidget->menuBar()->actionForPath("print");
	{
		auto *m = a_print->addMenuInto("startList", tr("&Start list"));
		{
			{
				auto *a = new qfw::Action(tr("&Classes"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_startListClasses);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("C&lubs"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_startListClubs);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("&Starters"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_startListStarters);
				m->addActionInto(a);
			}
			m->addSeparatorInto();
			{
				auto *a = new qfw::Action(tr("Classes n stages"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_startListClassesNStages);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("Clubs n stages"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_startListClubsNStages);
				m->addActionInto(a);
			}
		}
	}
	{
		auto *m = a_print->addMenuInto("results", tr("&Results"));
		{
			{
				auto *a = new qfw::Action(tr("&Current stage"));
				a->setShortcut("Ctrl+P");
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_resultsClasses);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("Current stage for speaker"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_resultsForSpeaker);
				m->addActionInto(a);
			}
			m->addSeparatorInto();
			{
				auto *a = new qfw::Action(tr("Current stage awards"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_resultsAwards);
				m->addActionInto(a);
			}
			m->addSeparatorInto();
			{
				auto *a = new qfw::Action(tr("&After n stages"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_resultsNStages);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("&After n stages for speaker"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_resultsNStagesSpeaker);
				m->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("N stages awards"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::report_nStagesAwards);
				m->addActionInto(a);
			}
		}
	}
	a_print->addSeparatorInto();
	{
		auto *a = new qfw::Action(tr("&Competitors with rented cards"));
		connect(a, &qfw::Action::triggered, [this]() {
			qff::MainWindow *fwk = qff::MainWindow::frameWork();
			QVariantMap props;
			props["stageId"] = selectedStageId();
			qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
										, manifest()->homeDir() + "/reports/competitorsWithCardRent.qml"
										, QVariant()
										, tr("Competitors with rented cards")
										, "printReport"
										, props
										);
		});
		a_print->addActionInto(a);
	}
	auto *a_export = m_partWidget->menuBar()->actionForPath("export");
	{
		auto *m_stlist = a_export->addMenuInto("startList", tr("&Start list"));
		auto *m_html = m_stlist->addMenuInto("html", tr("&HTML"));
		{
			{
				auto *a = new qfw::Action(tr("&Classes"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::export_startListClassesHtml);
				m_html->addActionInto(a);
			}
			{
				auto *a = new qfw::Action(tr("C&lubs"));
				connect(a, &qfw::Action::triggered, this, &RunsPlugin::export_startListClubsHtml);
				m_html->addActionInto(a);
			}
		}
		auto *m_xml = m_stlist->addMenuInto("html", tr("&XML"));
		{
			auto *a = new qfw::Action(tr("&IOF-XML 3.0"));
			connect(a, &qfw::Action::triggered, tpw->runsWidget(), &RunsWidget::export_startList_iofxml30_stage);
			m_xml->addActionInto(a);
		}
	}
	/*
	var a_export = root.partWidget.menuBar.actionForPath("export", false);

	var m_stlist = a_export.addMenuInto("startList", qsTr("&Start list"));
	a = m_stlist.addMenuInto("html", "&HTML");
	a.addActionInto(act_export_html_startList_classes);
	a.addActionInto(act_export_html_startList_clubs);
	a = m_stlist.addMenuInto("xml", "&XML");
	a.addActionInto(act_export_startList_iofxml3);

	//var m_results = a_export.addMenuInto("results", "&Results");
	var m_results = root.partWidget.menuBar.actionForPath("export/results", false);
	var action_results_export_iofxml_23 = root.partWidget.menuBar.actionForPath("export/results/iofxml23", false);
	if(action_results_export_iofxml_23) {
		action_results_export_iofxml_23.triggered.connect(function () {
			var default_file_name = "results-iof.xml";
			var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("XML files (*.xml)"));
			if(file_name)
				results.exportIofXml2(file_name)
		});
	}
	//m_results.addActionInto(act_export_results_iofxml_23);
	//m_results.addActionInto(act_export_results_iofxml_30);
	m_results.addActionInto(act_export_results_winsplits);
	*/
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

static int latlng_distance(double lat1, double lng1, double lat2, double lng2)
{
	/// http://www.movable-type.co.uk/scripts/latlong.html
	if(qFuzzyIsNull(lng2 - lng1) && qFuzzyIsNull(lat2 - lat1))
		return 0;
	auto deg2rad = [](double deg) {
		static constexpr double PI = 3.1415926535;
		return deg * PI / 180;
	};
	lat1 = deg2rad(lat1);
	lat2 = deg2rad(lat2);
	lng1 = deg2rad(lng1);
	lng2 = deg2rad(lng2);
	double x = (lng2 - lng1) * std::cos((lat1 + lat2) / 2);
	double y = (lat2 - lat1);
	static constexpr double R = 6371000;
	double d = std::sqrt(x*x + y*y) * R;
	return static_cast<int>(std::ceil(d));
}

QVariantMap RunsPlugin::courseCodesForRunId(int run_id)
{
	qfLogFuncFrame() << "run id:" << run_id;
	QVariantMap ret;
	if(run_id <= 0) {
		qfError() << "Run ID == 0";
		return ret;
	}
	int course_id = courseForRun(run_id);
	if(course_id <= 0) {
		qfError() << "Course ID == 0";
		return ret;
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("courses", "*")
				.from("courses")
				.where("courses.id=" QF_IARG(course_id));
		qfs::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		if(q.next())
			ret = q.values();
	}
	quickevent::core::si::CodeDef start_code;
	QVariantList codes;
	quickevent::core::si::CodeDef finish_code;
	{
		qfs::QueryBuilder qb;
		qb.select2("coursecodes", "position")
				.select2("codes", "*")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(course_id))
				.orderBy("coursecodes.position");
		qfs::Query q;
		//qfWarning() << qb.toString();
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while (q.next()) {
			quickevent::core::si::CodeDef cd(q.values());
			const QString control_type = cd.type();
			if(control_type == quickevent::core::si::CodeDef::CONTROL_TYPE_START) {
				start_code = cd;
			}
			else if(control_type == quickevent::core::si::CodeDef::CONTROL_TYPE_FINISH) {
				finish_code = cd;
				// whatever code is imported, QE is using 999 everywhere
				finish_code.setCode(quickevent::core::si::PunchRecord::FINISH_PUNCH_CODE);
			}
			else if(control_type.isEmpty()) {
				codes << cd;
			}
		}
	}
	if (finish_code.isEmpty()) {
		finish_code.setCode(quickevent::core::si::PunchRecord::FINISH_PUNCH_CODE);
		finish_code.setType(quickevent::core::si::CodeDef::CONTROL_TYPE_FINISH);
	}
	int course_len = 0;
	quickevent::core::si::CodeDef prev_cd = start_code;
	for (int i = 0; i < codes.count(); ++i) {
		quickevent::core::si::CodeDef cd(codes[i].toMap());
		int d = latlng_distance(prev_cd.latitude(), prev_cd.longitude(), cd.latitude(), cd.longitude());
		course_len += d;
		cd.setDistance(d);
		codes[i] = cd;
		qfDebug() << "pos:" << (i+1) << prev_cd.toString() << "-->" << cd.toString() << "distance:" << d;
		prev_cd = cd;
	}
	{
		int d = latlng_distance(prev_cd.latitude(), prev_cd.longitude(), finish_code.latitude(), finish_code.longitude());
		course_len += d;
		qfDebug() << "finish distance:" << d;
		finish_code.setDistance(d);
	}
	qfDebug() << "course len:" << course_len;
	ret["codes"] = codes;
	ret["finishCode"] = finish_code;
	return ret;
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

qf::core::utils::Table RunsPlugin::nstagesClassResultsTable(int stages_count, int class_id, int places, bool exclude_disq)
{
	qfs::QueryBuilder qb;
	qb.select2("competitors", "id, registration, licence")
			.select2("clubs","name")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("competitors")
			.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
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

qf::core::utils::TreeTable RunsPlugin::nstagesResultsTable(int stages_count, int places, bool exclude_disq)
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
		//qfInfo() << "Processing class:" << tt_row.value("name").toString();
		int class_id = tt_row.value("id").toInt();
		qf::core::utils::Table t = nstagesClassResultsTable(stages_count, class_id, places, exclude_disq);
		qf::core::utils::TreeTable tt2 = t.toTreeTable();
		tt_row.appendTable(tt2);
	}
	tt.setValue("stagesCount", stages_count);
	return tt;
}

QVariant RunsPlugin::nstagesResultsTableData(int stages_count, int places, bool exclude_disq)
{
	return nstagesResultsTable(stages_count, places, exclude_disq).toVariant();
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
			int run_id = row2.value("runs.id").toInt();
			int stime = row2.value("startTimeMs").toInt();
			int ftime = row2.value("finishTimeMs").toInt();
			int time = row2.value("timeMs").toInt();
			//qfInfo() << row1.value("classes.name").toString() << row2.value("competitors.lastName").toString() << stime << ftime << time;
			if(ftime && time)
				stime = ftime - time; // cover cases when competitor didn't started according to start list from any reason
			result.insert(result.count(), QVariantList{"StartTime", stage_start_date_time.addMSecs(stime).toString(Qt::ISODate)});
			result.insert(result.count(), QVariantList{"FinishTime", stage_start_date_time.addMSecs(ftime).toString(Qt::ISODate)});
			result.insert(result.count(), QVariantList{"Time", time / 1000});

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
				competitor_status = "NotCompeting";
			if (competitor_status == STAT_OK) {
				// The position in the result list for the person that the result belongs to.
				// This element should only be present when the Status element is set to OK.
				result.insert(result.count(), QVariantList{"Position", row2.value("npos")});
			}
			result.insert(result.count(), QVariantList{"Status", competitor_status});

			int course_id = courseForRun(run_id);
			struct SplitTime
			{
				int code = 0;
				int time = 0;
				QString status;

				SplitTime(int c = 0) : code(c) {}
			};
			QMap<int, QVector<SplitTime>> course_codes;
			if(!course_codes.contains(course_id)) {
				qf::core::sql::QueryBuilder qb;
				qb.select2("codes", "code")
					.from("coursecodes")
					.join("coursecodes.codeId", "codes.id")
					.where("coursecodes.courseId=" QF_IARG(course_id))
					.where("COALESCE(codes.type, '') = ''")
					.orderBy("coursecodes.position");
				qf::core::sql::Query q;
				q.exec(qb.toString(), qf::core::Exception::Throw);
				QVector<SplitTime> sts;
				while(q.next()) {
					QSqlRecord rec = q.record();
					sts << SplitTime{q.value(0).toInt()};
				}
				course_codes[course_id] = sts;
			}
			QVector<SplitTime> &codes = course_codes[course_id];
			{
				for (int i = 0; i < codes.count(); ++i)
					codes[i].status = QStringLiteral("Missing");

				qfs::QueryBuilder qb;
				qb.select2("runlaps", "*")
					.from("runlaps")
					.where("runlaps.runId=" + QString::number(row2.value("runs.id").toInt()))
					.where("runlaps.code<" QF_IARG(quickevent::core::si::PunchRecord::FINISH_PUNCH_CODE) )
					//.where("runlaps.position >= 1")
					.orderBy("runlaps.position");

				qfs::Query q;
				q.exec(qb.toString());
				while(q.next()) {
					int ix = q.value(QStringLiteral("position")).toInt() - 1;
					if(ix < 0 || ix >= codes.count()) {
						qfWarning() << "runlap position out of codes range. index:" << ix << "codes count:" << codes.count();
						continue;
					}
					SplitTime &sts = codes[ix];
					sts.time = q.value("stpTimeMs").toInt() / 1000;
					sts.status = QString();
				}
			}
			for(const SplitTime &sts : codes) {
				QVariantList split_time{QStringLiteral("SplitTime"),
					QVariantList{QStringLiteral("ControlCode"), sts.code },
					QVariantList{QStringLiteral("Time"), sts.time },
				};
				if(!sts.status.isEmpty())
					split_time.insert(1, QVariantMap{ {QStringLiteral("status"), sts.status} });
				result.insert(result.count(), split_time);
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
		qf::core::utils::Table tt = nstagesClassResultsTable(stage_count, class_id, -1, false);
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

qf::core::utils::TreeTable RunsPlugin::startListClassesTable(const QString &where_expr, bool insert_vacants)
{
	auto *event_plugin = eventPlugin();
	int stage_id = selectedStageId();

	qfs::QueryBuilder qb;
	qb.select2("classes", "id, name")
		.select2("classdefs", "startTimeMin, lastStartTimeMin, startIntervalMin")
		.select2("courses", "length, climb")
		.from("classes")
		.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
		.join("classdefs.courseId", "courses.id")
		.orderBy("classes.name");//.limit(1);
	if(!where_expr.isEmpty())
		qb.where(where_expr);
	QVariantMap qpm;
	qpm["stage_id"] = stage_id;
	qf::core::model::SqlTableModel m;
	m.setQueryBuilder(qb);
	m.setQueryParameters(qpm);
	m.reload();
	auto tt = m.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	tt.setValue("stageStart", event_plugin->stageStartDateTime(stage_id));

	qfs::QueryBuilder qb2;
	qb2.select2("competitors", "lastName, firstName, registration, startNumber")
		.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
		.select2("runs", "siId, startTimeMs")
		.from("competitors")
		.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "INNER JOIN")
		.where("competitors.classId={{class_id}}")
		.orderBy("runs.startTimeMs, competitors.lastName");
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		int class_id = tt_row.value(QStringLiteral("classes.id")).toInt();
		//console.debug("class id:", class_id);
		qpm["class_id"] = class_id;
		qf::core::model::SqlTableModel m2;
		m2.setQueryBuilder(qb2);
		m2.setQueryParameters(qpm);
		m2.reload();
		auto tt2 = m2.toTreeTable();
		int start_time_0 = tt_row.value(QStringLiteral("startTimeMin")).toInt() * 60 * 1000;
		int start_time_last = tt_row.value(QStringLiteral("lastStartTimeMin")).toInt() * 60 * 1000;
		int start_interval = tt_row.value(QStringLiteral("startIntervalMin")).toInt() * 60 * 1000;
		if(start_interval > 0 && insert_vacants) {
			for(int j=0; j<tt2.rowCount(); j++) {
				qf::core::utils::TreeTableRow tt2_row = tt2.row(j);
				int start_time = tt2_row.value(QStringLiteral("startTimeMs")).toInt();
				//console.info(j, "t0:", start_time_0, start_time_0/60/1000, "start:", start_time, start_time/60/1000)
				while(start_time_0 < start_time) {
					// insert vakant row
					//qfInfo() << "adding row:" << j << (start_time_0 / 60 / 1000);
					qf::core::utils::TreeTableRow new_row = tt2.insertRow(j);
					new_row.setValue(QStringLiteral("startTimeMs"), start_time_0);
					new_row.setValue(QStringLiteral("competitorName"), "---");
					new_row.setValue(QStringLiteral("registration"), "");
					new_row.setValue(QStringLiteral("siId"), 0);
					new_row.setValue(QStringLiteral("startNumber"), 0);
					start_time_0 += start_interval;
					j++;
				}
				start_time_0 += start_interval;
			}
			while(start_time_0 <= start_time_last) {
				// insert vakants after
				qf::core::utils::TreeTableRow new_row = tt2.appendRow();
				new_row.setValue(QStringLiteral("startTimeMs"), start_time_0);
				new_row.setValue(QStringLiteral("competitorName"), "---");
				new_row.setValue(QStringLiteral("registration"), QString());
				new_row.setValue(QStringLiteral("siId"), 0);
				new_row.setValue(QStringLiteral("startNumber"), 0);
				start_time_0 += start_interval;
			}
		}
		tt_row.appendTable(tt2);
	}
	//qfInfo().noquote() << tt.toString();
	return tt;

}

qf::core::utils::TreeTable RunsPlugin::startListClubsTable()
{
	auto *event_plugin = eventPlugin();
	int stage_id = selectedStageId();

	QString qs1 = "SELECT COALESCE(substr(registration, 1, 3), '') AS clubAbbr FROM competitors GROUP BY clubAbbr ORDER BY clubAbbr";
	QString qs = "SELECT t2.clubAbbr, clubs.name FROM ( " + qs1 + " ) AS t2"
			+ " LEFT JOIN clubs ON t2.clubAbbr=clubs.abbr"
			+ " ORDER BY t2.clubAbbr";
	qf::core::model::SqlTableModel m;
	m.setQuery(qs);
	m.reload();
	auto tt = m.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	tt.setValue("stageStart", event_plugin->stageStartDateTime(stage_id));
	tt.columns().column(0).setType(QVariant::String); // sqlite returns clubAbbr column as QVariant::Invalid, set correct type
	//console.info(tt.toString());

	qfs::QueryBuilder qb;
	qb.select2("competitors", "registration, startNumber")
		.select("COALESCE(competitors.lastName, '') || ' '  || COALESCE(competitors.firstName, '') AS competitorName")
		.select("lentcards.siid IS NOT NULL OR runs.cardLent AS cardLent")
		.select2("classes", "name")
		.select2("runs", "siId, startTimeMs")
		.from("competitors")
		.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "INNER JOIN")
		.joinRestricted("runs.siid", "lentcards.siid", "NOT lentcards.ignored")
		.join("competitors.classId", "classes.id")
		.where("COALESCE(substr(competitors.registration, 1, 3), '')='{{club_abbr}}'")
		.orderBy("classes.name, runs.startTimeMs, competitors.lastName");
	QVariantMap qpm;
	qpm["stage_id"] = stage_id;
	qf::core::model::SqlTableModel m2;
	m2.setQueryBuilder(qb);
	m2.setQueryParameters(qpm);
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		QString club_abbr = tt_row.value("clubAbbr").toString();
		//console.debug("club_abbr:", club_abbr);
		qpm["club_abbr"] = club_abbr;
		m2.setQueryParameters(qpm);
		m2.reload();
		//console.info(reportModel.effectiveQuery());
		auto tt2 = m2.toTreeTable();
		tt_row.appendTable(tt2);
	}
	return tt;
}

qf::core::utils::TreeTable RunsPlugin::startListStartersTable(const QString &where_expr)
{
	auto *event_plugin = eventPlugin();
	int stage_id = selectedStageId();

	qfs::QueryBuilder qb;
	qb.select2("competitors", "registration, id, startNumber")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select("COALESCE(runs.startTimeMs / 1000 / 60, 0) AS startTimeMin")
			.select2("runs", "siId, startTimeMs")
			.select2("classes", "name")
			.from("competitors")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND runs.isRunning", "INNER JOIN")
			.join("competitors.classId", "classes.id")
			.orderBy("runs.startTimeMs, classes.name, competitors.lastName");//.limit(1);
	if(!where_expr.isEmpty())
		qb.where(where_expr);
	QVariantMap qpm;
	qpm["stage_id"] = stage_id;
	qf::core::model::SqlTableModel m;
	m.setQueryBuilder(qb);
	m.setQueryParameters(qpm);
	m.reload();
	auto tt = m.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	tt.setValue("stageStart", event_plugin->stageStartDateTime(stage_id));
	return tt;
}

qf::core::utils::TreeTable RunsPlugin::startListClassesNStagesTable(int stages_count, const QString &where_expr)
{
	auto *event_plugin = eventPlugin();
	int stage_id = selectedStageId();

	qfs::QueryBuilder qb;
	qb.select2("classes", "id, name")
			.from("classes")
			.orderBy("classes.name");
	if(!where_expr.isEmpty())
		qb.where(where_expr);
	qf::core::model::SqlTableModel m;
	m.setQueryBuilder(qb);
	m.reload();
	auto tt = m.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	tt.setValue("stageStart", event_plugin->stageStartDateTime(stage_id));
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		int class_id = tt_row.value("classes.id").toInt();
		qfs::QueryBuilder qb2;
		qb2.select2("competitors", "registration, lastName, firstName, siId, startNumber")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("competitors")
			.where("competitors.classId={{class_id}}")
			.orderBy("competitors.lastName, competitors.registration");
		for(int stage_id = 1; stage_id <= stages_count; stage_id++) {
			QString runs_table = "runs" + QString::number(stage_id);
			qb2.select2(runs_table, "siid, startTimeMs, isRunning")
				.joinRestricted("competitors.id"
								, "runs.competitorId AS " + runs_table, runs_table + ".stageId=" + QString::number(stage_id) + " AND " + runs_table + ".isRunning");
		}
		QVariantMap qpm;
		qpm["class_id"] = class_id;
		qf::core::model::SqlTableModel m2;
		m2.setQueryBuilder(qb2);
		m2.setQueryParameters(qpm);
		//qfInfo() << m2.effectiveQuery();
		m2.reload();
		tt_row.appendTable(m2.toTreeTable());
	}
	return tt;
}

qf::core::utils::TreeTable RunsPlugin::startListClubsNStagesTable(int stages_count)
{
	auto *event_plugin = eventPlugin();
	int stage_id = selectedStageId();

	QString qs1 = "SELECT COALESCE(substr(registration, 1, 3), '') AS clubAbbr FROM competitors GROUP BY clubAbbr ORDER BY clubAbbr";
	QString qs = "SELECT t2.clubAbbr, clubs.name FROM ( " + qs1 + " ) AS t2"
			+ " LEFT JOIN clubs ON t2.clubAbbr=clubs.abbr"
			+ " ORDER BY t2.clubAbbr";
	qf::core::model::SqlTableModel m;
	m.setQuery(qs);
	m.reload();
	auto tt = m.toTreeTable();
	tt.setValue("stageId", stage_id);
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	tt.setValue("stageStart", event_plugin->stageStartDateTime(stage_id));
	tt.columns().column(0).setType(QVariant::String); // sqlite returns clubAbbr column as QVariant::Invalid, set correct type
	//console.info(tt.toString());
	for(int i=0; i<tt.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt_row = tt.row(i);
		QString club_abbr = tt_row.value("clubAbbr").toString();
		qfs::QueryBuilder qb2;
		qb2.select2("competitors", "registration, siId, startNumber")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.select2("classes", "name")
				.from("competitors")
				.join("competitors.classId", "classes.id")
				.where("substr(competitors.registration, 1, 3)='{{club_abbr}}'")
				.orderBy("classes.name, competitors.lastName");
		for(int stage_id = 1; stage_id <= stages_count; stage_id++) {
			QString runs_table = "runs" + QString::number(stage_id);
			qb2.select2(runs_table, "siid, startTimeMs, isRunning")
				.joinRestricted("competitors.id"
								, "runs.competitorId AS " + runs_table, runs_table + ".stageId=" + QString::number(stage_id) + " AND " + runs_table + ".isRunning");
		}
		QVariantMap qpm;
		qpm["club_abbr"] = club_abbr;
		qf::core::model::SqlTableModel m2;
		m2.setQueryBuilder(qb2);
		m2.setQueryParameters(qpm);
		//qfInfo() << m2.effectiveQuery();
		m2.reload();
		tt_row.appendTable(m2.toTreeTable());
	}
	return tt;
}

void RunsPlugin::report_startListClasses()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("startListClassesReportOptions");
	dlg.loadPersistentSettings();
	dlg.setStartListOptionsVisible(true);
	dlg.setPageLayoutVisible(false);
	if(dlg.exec()) {
		auto tt = startListClassesTable(dlg.sqlWhereExpression(), dlg.isStartListPrintVacants());
		auto opts = dlg.optionsMap();
		QVariantMap props;
		props["options"] = opts;
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/startList_classes.qml"
									, tt.toVariant()
									, tr("Start list by classes")
									, "printStartList"
									, props
									);

	}
}

void RunsPlugin::report_startListClubs()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("startListClubsReportOptions");
	dlg.loadPersistentSettings();
	dlg.setClassFilterVisible(false);
	dlg.setStartListOptionsVisible(false);
	dlg.setPageLayoutVisible(false);
	if(dlg.exec()) {
		auto tt = startListClubsTable();
		auto opts = dlg.optionsMap();
		QVariantMap props;
		props["options"] = opts;
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/startList_clubs.qml"
									, tt.toVariant()
									, tr("Start list by clubs")
									, "printStartList"
									, props
									);
	}
}

void RunsPlugin::report_startListStarters()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("startListStartersReportOptions");
	dlg.loadPersistentSettings();
	dlg.setClassFilterVisible(true);
	dlg.setStartListOptionsVisible(true);
	dlg.setStartersOptionsVisible(true);
	if(dlg.exec()) {
		auto tt = startListStartersTable(dlg.sqlWhereExpression());
		auto opts = dlg.optionsMap();
		QVariantMap props;
		props["options"] = opts;
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/startList_starters.qml"
									, tt.toVariant()
									, tr("Start list for starters")
									, "printStartList"
									, props
									);
	}
}

void RunsPlugin::report_startListClassesNStages()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("startListClassesNStagesReportOptions");
	dlg.loadPersistentSettings();

	dlg.setStagesCount(event_plugin->stageCount());
	dlg.setStartListOptionsVisible(true);
	dlg.setVacantsVisible(false);
	dlg.setStagesOptionVisible(true);
	dlg.setClassFilterVisible(true);
	dlg.setColumnCountEnable(false);
	if(dlg.exec()) {
		auto tt = startListClassesNStagesTable(dlg.stagesCount(), dlg.sqlWhereExpression());
		auto opts = dlg.optionsMap();
		//QString report_title = tr("Start list by classes after %1 stages").arg(dlg.stagesCount());
		QVariantMap props;
		props["options"] = opts;
		//props["reportTitle"] = "report_title";
		//qfInfo() << props;
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/startList_classes_nstages.qml"
									, tt.toVariant()
									, tr("Start list by classes after %1 stages").arg(dlg.stagesCount())
									, "printStartList"
									, props
									);
	}
}

void RunsPlugin::report_startListClubsNStages()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("startListClubsNStagesReportOptions");
	dlg.loadPersistentSettings();

	dlg.setStagesCount(event_plugin->stageCount());
	dlg.setStartListOptionsVisible(true);
	dlg.setVacantsVisible(false);
	dlg.setStagesOptionVisible(true);
	dlg.setClassFilterVisible(true);
	dlg.setColumnCountEnable(false);
	if(dlg.exec()) {
		auto tt = startListClubsNStagesTable(dlg.stagesCount());
		auto opts = dlg.optionsMap();
		//QString report_title = tr("Start list by classes after %1 stages").arg(dlg.stagesCount());
		QVariantMap props;
		props["options"] = opts;
		//props["reportTitle"] = "report_title";
		//qfInfo() << props;
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/startList_clubs_nstages.qml"
									, tt.toVariant()
									, tr("Start list by clubs after %1 stages").arg(dlg.stagesCount())
									, "printStartList"
									, props
									);
	}
}

void RunsPlugin::report_resultsClasses()
{
	//auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("resultsClassesReportOptions");
	dlg.loadPersistentSettings();
	dlg.setResultOptionsVisible(true);
	//dlg.setPageLayoutVisible(false);
	if(dlg.exec()) {
		auto tt = currentStageResultsTable(dlg.sqlWhereExpression(), dlg.resultNumPlaces());
		auto opts = dlg.optionsMap();
		QVariantMap props;
		props["options"] = opts;
		//props["stageCount"] = event_plugin->eventConfig()->stageCount();
		//props["stageNumber"] = selectedStageId();
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/results_stage.qml"
									, tt.toVariant()
									, tr("Results by classes")
									, "printResults"
									, props
									);

	}
}

void RunsPlugin::report_resultsForSpeaker()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	quickevent::gui::ReportOptionsDialog dlg(fwk);
	dlg.setPersistentSettingsId("resultsClassesSpeakerReportOptions");
	dlg.loadPersistentSettings();
	dlg.setResultOptionsVisible(true);
	//dlg.setPageLayoutVisible(false);
	if(dlg.exec()) {
		auto tt = currentStageResultsTable(dlg.sqlWhereExpression(), dlg.resultNumPlaces());
		auto opts = dlg.optionsMap();
		QVariantMap props;
		props["options"] = opts;
		//props["stageCount"] = event_plugin->eventConfig()->stageCount();
		//props["stageNumber"] = selectedStageId();
		qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
									, manifest()->homeDir() + "/reports/results_stageSpeaker.qml"
									, tt.toVariant()
									, tr("Results by classes")
									, "printResultsSpeaker"
									, props
									);
	}
}

void RunsPlugin::report_resultsAwards()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QVariantMap opts;
	opts["stageId"] = event_plugin->currentStageId();
	opts = printAwardsOptionsWithDialog(opts);
	QString rep_path = opts.value("reportPath").toString();
	if(rep_path.isEmpty())
		return;

	QVariantMap props;
	props["eventConfig"] = QVariant::fromValue(event_plugin->eventConfig());
	auto tt = stageResultsTable(opts.value("stageId").toInt(), QString(), opts.value("numPlaces").toInt());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
								, rep_path
								, tt.toVariant()
								, tr("Stage awards")
								, "printResultsAwards"
								, props
								);
}

void RunsPlugin::report_resultsNStages()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	NStagesReportOptionsDialog dlg(fwk);
	//dlg.setPersistentSettingsId("resultsReportOptionsNStagesWide");
	dlg.setStagesCount(event_plugin->currentStageId());
	dlg.setMaxPlacesCount(9999);
	dlg.setExcludeDisqualified(true);
	if(!dlg.exec())
		return;
	auto tt = nstagesResultsTable(dlg.stagesCount(), dlg.maxPlacesCount(), dlg.isExcludeDisqualified());
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	//tt.setValue("stageStart", event_plugin->stageStartDateTime(stages_count));
	QVariantMap props;
	props["stagesCount"] = dlg.stagesCount();
	qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
								, manifest()->homeDir() + "/reports/results_nstages.qml"
								, tt.toVariant()
								, tr("Results after %1 stages").arg(dlg.stagesCount())
								, "printResultsNStages"
								, props
							  );
}

void RunsPlugin::report_resultsNStagesSpeaker()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	NStagesReportOptionsDialog dlg(fwk);
	//dlg.setPersistentSettingsId("resultsReportOptionsNStagesWide");
	dlg.setStagesCount(event_plugin->currentStageId());
	dlg.setMaxPlacesCount(9999);
	dlg.setExcludeDisqualified(true);
	if(!dlg.exec())
		return;
	auto tt = nstagesResultsTable(dlg.stagesCount(), dlg.maxPlacesCount(), dlg.isExcludeDisqualified());
	tt.setValue("event", event_plugin->eventConfig()->value("event"));
	//tt.setValue("stageStart", event_plugin->stageStartDateTime(stages_count));
	QVariantMap props;
	props["stagesCount"] = dlg.stagesCount();
	qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
								, manifest()->homeDir() + "/reports/results_nstagesSpeaker.qml"
								, tt.toVariant()
								, tr("Results after %1 stages").arg(dlg.stagesCount())
								, "printResultsNStagesWide"
								, props
														  );
}

void RunsPlugin::report_nStagesAwards()
{
	auto *event_plugin = eventPlugin();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QVariantMap opts;
	opts["stageId"] = event_plugin->currentStageId();
	opts = printAwardsOptionsWithDialog(opts);
	QString rep_path = opts.value("reportPath").toString();
	if(rep_path.isEmpty())
		return;

	QVariantMap props;
	props["eventConfig"] = QVariant::fromValue(event_plugin->eventConfig());
	auto tt = nstagesResultsTable(opts.value("stageId").toInt(), opts.value("numPlaces").toInt());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(fwk
								, rep_path
								, tt.toVariant()
								, tr("Awards after %1 stages").arg(opts.value("numPlaces").toInt())
								, "printResultsAwardsNStages"
								, props
								);
}

static void append_list(QVariantList &lst, const QVariantList &new_lst)
{
	lst.insert(lst.count(), new_lst);
}

void RunsPlugin::export_startListClassesHtml()
{
	qf::core::utils::TreeTable tt1 = startListClassesTable("", false);
	QVariantList body{QStringLiteral("body")};
	QString h1_str = "{{documentTitle}}";
	QVariantMap event = tt1.value("event").toMap();
	if(event.value("stageCount").toInt() > 1)
		h1_str = "E" + tt1.value("stageId").toString() + " " + h1_str;
	append_list(body, QVariantList{"h1", h1_str});
	append_list(body, QVariantList{"h2", event.value("name")});
	append_list(body, QVariantList{"h3", event.value("place")});
	append_list(body, QVariantList{"h3", tt1.value("stageStart")});
	QVariantList div1{"div"};
	for(int i=0; i<tt1.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt1_row = tt1.row(i);
		append_list(div1,
					QVariantList{
						"a",
						QVariantMap{{"href", "#class_" + tt1_row.value("classes.name").toString()}},
						tt1_row.value("classes.name"),
						"&nbsp;"
					});
	}
	append_list(body, div1);
	for(int i=0; i<tt1.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt1_row = tt1.row(i);
		div1 = QVariantList{
				"h2",
				QVariantList{
					"a",
					QVariantMap{{"name", "class_" + tt1_row.value(QStringLiteral("classes.name")).toString()}},
					tt1_row.value("classes.name")
				}
			};
		append_list(body, div1);
		div1 = QVariantList{
				"h3",
				tr("length:"),
				tt1_row.value("courses.length"), " ", tr("climb:"), tt1_row.value("courses.climb")
			};
		append_list(body, div1);
		QVariantList table{"table"};
		qf::core::utils::TreeTable tt2 = tt1_row.table();
		QVariantList trr{"tr",
				  QVariantList{"th", tr("Start")},
				  QVariantList{"th", tr("Name")},
				  QVariantList{"th", tr("Registration")},
				  QVariantList{"th", tr("SI")}
				};
		append_list(table, trr);
		for(int j=0; j<tt2.rowCount(); j++) {
			qf::core::utils::TreeTableRow tt2_row = tt2.row(j);
			QVariantList trr{"tr"};
			if(j % 2)
				trr << QVariantMap{{"class", "odd"}};
			append_list(trr, QVariantList{"td", quickevent::core::og::TimeMs(tt2_row.value("startTimeMs").toInt()).toString()});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("competitorName"))});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("registration"))});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("runs.siId"))});
			append_list(table, trr);
		}
		append_list(body, table);
	}
	QString file_name = QDir::tempPath() + "/quickevent/e" + tt1.value("stageId").toString();
	if(QDir().mkpath(file_name)) {
		QString default_file_name = "startlist-classes.html";
		file_name += "/" + default_file_name;
		QVariantMap options;
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("Start list by classes"));
		QString str = qf::core::utils::HtmlUtils::fromHtmlList(body, opts);
		QFile f(file_name);
		if(f.open(QFile::WriteOnly)) {
			f.write(str.toUtf8());
			f.close();
			qfInfo() << "exported:" << file_name;
			QDesktopServices::openUrl(QUrl::fromLocalFile(file_name));
		}
	}
}

void RunsPlugin::export_startListClubsHtml()
{
	qf::core::utils::TreeTable tt1 = startListClubsTable();
	QVariantList body{QStringLiteral("body")};
	QString h1_str = "{{documentTitle}}";
	QVariantMap event = tt1.value("event").toMap();
	if(event.value("stageCount").toInt() > 1)
		h1_str = "E" + tt1.value("stageId").toString() + " " + h1_str;
	append_list(body, QVariantList{"h1", h1_str});
	append_list(body, QVariantList{"h2", event.value("name")});
	append_list(body, QVariantList{"h3", event.value("place")});
	append_list(body, QVariantList{"h3", tt1.value("stageStart")});
	QVariantList div1{"div"};
	for(int i=0; i<tt1.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt1_row = tt1.row(i);
		append_list(div1,
					QVariantList{
						"a",
						QVariantMap{{"href", "#club_" + tt1_row.value("clubAbbr").toString()}},
						tt1_row.value("clubAbbr"),
						"&nbsp;"
					});
	}
	append_list(body, div1);
	for(int i=0; i<tt1.rowCount(); i++) {
		qf::core::utils::TreeTableRow tt1_row = tt1.row(i);
		div1 = QVariantList{
				"h2",
				QVariantList{
					"a",
					QVariantMap{{"name", "club_" + tt1_row.value(QStringLiteral("clubAbbr")).toString()}},
					tt1_row.value("clubAbbr")
				}
			};
		append_list(body, div1);
		div1 = QVariantList{
				"h3", tt1_row.value("name")
			};
		append_list(body, div1);
		QVariantList table{"table"};
		qf::core::utils::TreeTable tt2 = tt1_row.table();
		QVariantList trr{"tr",
				QVariantList{"th", tr("Start")},
				QVariantList{"th", tr("Class")},
				QVariantList{"th", tr("Name")},
				QVariantList{"th", tr("Registration")},
				QVariantList{"th", tr("SI")}
			};
		append_list(table, trr);
		for(int j=0; j<tt2.rowCount(); j++) {
			qf::core::utils::TreeTableRow tt2_row = tt2.row(j);
			QVariantList trr{"tr"};
			if(j % 2)
				trr << QVariantMap{{"class", "odd"}};
			append_list(trr, QVariantList{"td", quickevent::core::og::TimeMs(tt2_row.value("startTimeMs").toInt()).toString()});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("classes.name"))});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("competitorName"))});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("registration"))});
			append_list(trr, QVariantList{"td", tt2_row.value(QStringLiteral("runs.siId"))});
			append_list(table, trr);
		}
		append_list(body, table);
	}
	QString file_name = QDir::tempPath() + "/quickevent/e" + tt1.value("stageId").toString();
	if(QDir().mkpath(file_name)) {
		QString default_file_name = "startlist-clubs.html";
		file_name += "/" + default_file_name;
		QVariantMap options;
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
		opts.setDocumentTitle(tr("Start list by clubs"));
		QString str = qf::core::utils::HtmlUtils::fromHtmlList(body, opts);
		QFile f(file_name);
		if(f.open(QFile::WriteOnly)) {
			f.write(str.toUtf8());
			f.close();
			qfInfo() << "exported:" << file_name;
			QDesktopServices::openUrl(QUrl::fromLocalFile(file_name));
		}
	}
}

bool RunsPlugin::exportStartListStageIofXml30(int stage_id, const QString &file_name)
{
	auto *event_plugin = eventPlugin();
	QDateTime start00_datetime = event_plugin->stageStartDateTime(stage_id);
	//console.info("start00_datetime:", start00_datetime, typeof start00_datetime)
	auto start00_epoch_sec = start00_datetime.toSecsSinceEpoch();
	Event::EventConfig *event_config = event_plugin->eventConfig();
	bool last_handicap_stage = event_config->stageCount() == selectedStageId() && event_config->isHandicap();
	bool print_vacants = !last_handicap_stage;
	//console.debug("print_vacants", print_vacants);
	auto tt1 = startListClassesTable("", print_vacants);

	QVariantList xml_root{"StartList" ,
		QVariantMap {
			{"xmlns", "http://www.orienteering.org/datastandard/3.0"},
			{"xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"},
			{"iofVersion", "3.0"},
			{"creator", "QuickEvent"},
			{"createTime", QDateTime::currentDateTimeUtc().toString(Qt::ISODate)}
		}
	};

	//var event = tt1.value("event");
	QVariantList xml_event{"Event"};
	append_list(xml_event, QVariantList{"Name", event_config->eventName()});
	append_list(xml_event, QVariantList{"StartTime", QVariantList{"Date", event_config->eventDateTime().toUTC().date().toString(Qt::ISODate)}, QVariantList{"Time", event_config->eventDateTime().time().toString(Qt::ISODate)}});
	append_list(xml_event, QVariantList{"EndTime", QVariantList{"Date", event_config->eventDateTime().toUTC().date().toString(Qt::ISODate)}, QVariantList{"Time", event_config->eventDateTime().time().toString(Qt::ISODate)}});
	QStringList director = event_config->director().split(' ', QString::SkipEmptyParts);
	QStringList main_referee = event_config->mainReferee().split(' ', QString::SkipEmptyParts);
	append_list(xml_event, QVariantList{"Official", QVariantMap{{"type", "Director"}}, QVariantList{"Person", QVariantList{"Name", QVariantList{"Family", director.value(0)}, QVariantList{"Given", director.value(1)}}}});
	append_list(xml_event, QVariantList{"Official", QVariantMap{{"type", "MainReferee"}}, QVariantList{"Person", QVariantList{"Name", QVariantList{"Family", main_referee.value(0)}, QVariantList{"Given", main_referee.value(1)}}}});
	append_list(xml_root, xml_event);

	for(int i=0; i<tt1.rowCount(); i++) {
		auto tt1_row = tt1.row(i);
		QVariantList class_start{"ClassStart"};
		append_list(class_start, QVariantList{"Class", QVariantList{"Id", tt1_row.value(QStringLiteral("classes.id"))}, QVariantList{"Name", tt1_row.value(QStringLiteral("classes.name"))}});
		append_list(class_start, QVariantList{"Course", QVariantList{"Length", tt1_row.value(QStringLiteral("courses.length"))}, QVariantList{"Climb", tt1_row.value(QStringLiteral("courses.climb"))}});
		append_list(class_start, QVariantList{"StartName", "Start1"});
		qf::core::utils::TreeTable tt2 = tt1_row.table();
		int pos = 0;
		for(int j=0; j<tt2.rowCount(); j++) {
			pos++;
			auto tt2_row = tt2.row(j);
			QVariantList xml_person{"PersonStart"};
			QVariantList person{"Person"};
			append_list(person, QVariantList{"Id", tt2_row.value(QStringLiteral("competitors.registration"))});
			auto family = tt2_row.value(QStringLiteral("competitors.lastName"));
			auto given = tt2_row.value(QStringLiteral("competitors.firstName"));
			append_list(person, QVariantList{"Name", QVariantList{"Family", family}, QVariantList{"Given", given}});
			QVariantList xml_start{"Start"};
			int stime_msec = tt2_row.value("startTimeMs").toInt();
			//console.info(start00_datetime.toJSON(), start00_datetime.getHours(), start00_epoch_sec / 60 / 60);
			//console.info(family, given, start00_epoch_sec, stime_sec, stime_sec / 60);
			QDateTime stime_datetime = QDateTime::fromMSecsSinceEpoch(start00_epoch_sec * 1000 + stime_msec);
			//sdatetime.setTime(start00_epoch_sec);
			//console.warn(stime_datetime.toJSON());
			//stime_epoch_sec = start00_epoch_sec + stime_epoch_sec;
			append_list(xml_start, QVariantList{"StartTime", stime_datetime.toUTC().toString(Qt::ISODateWithMs)});
			append_list(xml_start, QVariantList{"ControlCard", tt2_row.value(QStringLiteral("runs.siId"))});
			append_list(xml_person, person);
			append_list(xml_person, xml_start);
			append_list(class_start, xml_person);
		}
		append_list(xml_root, class_start);
	}
	qf::core::utils::HtmlUtils::FromXmlListOptions opts;
	opts.setDocumentTitle(tr("E%1 IOF XML stage results").arg(tt1.value("stageId").toString()));
	QString str = qf::core::utils::HtmlUtils::fromXmlList(xml_root, opts);
	QFile f(file_name);
	if(f.open(QFile::WriteOnly)) {
		f.write(str.toUtf8());
		qfInfo() << "exported:" << file_name;
		return true;
	}
	return false;
}

}

