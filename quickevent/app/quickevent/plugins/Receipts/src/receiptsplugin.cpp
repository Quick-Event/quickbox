#include "receiptsplugin.h"
#include "receiptswidget.h"
#include "receiptsprinter.h"

#include <quickevent/core/si/readcard.h>
#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/og/timems.h>

#include <qf/core/utils/settings.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/model/sqltablemodel.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>
#include <qf/qmlwidgets/reports/processor/reportprocessor.h>
#include <qf/qmlwidgets/reports/processor/reportitem.h>
#include <qf/qmlwidgets/reports/processor/reportpainter.h>
#include <plugins/CardReader/src/cardreaderplugin.h>
#include <plugins/Event/src/eventplugin.h>
#include <quickevent/gui/partwidget.h>

#include <QDomDocument>
#include <QSqlRecord>
#include <QPrinterInfo>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;
using quickevent::gui::PartWidget;
using qff::getPlugin;
using Event::EventPlugin;
using Receipts::ReceiptsPlugin;
using CardReader::CardReaderPlugin;

namespace Receipts {

const QLatin1String ReceiptsPlugin::SETTINGS_PREFIX("plugins/Receipts");

ReceiptsPlugin::ReceiptsPlugin(QObject *parent)
	: Super("Receipts", parent)
{
	connect(this, &ReceiptsPlugin::installed, this, &ReceiptsPlugin::onInstalled);
}

void ReceiptsPlugin::onInstalled()
{
	qff::initPluginWidget<ReceiptsWidget, PartWidget>(tr("Receipts"), featureId());
}

QString ReceiptsPlugin::currentReceiptPath()
{
	QString printer_options_key = ReceiptsPlugin::SETTINGS_PREFIX;
	printer_options_key += "receipts/current";
	qf::core::utils::Settings settings;
	QString s = settings.value(printer_options_key).toString().trimmed();
	return s;
}

void ReceiptsPlugin::setCurrentReceiptPath(const QString &path)
{
	QString printer_options_key = ReceiptsPlugin::SETTINGS_PREFIX;
	printer_options_key += "receipts/current";
	qf::core::utils::Settings settings;
	qfInfo() << "setting current receipt path to:" << path;
	settings.setValue(printer_options_key, path);
}

ReceiptsPrinterOptions ReceiptsPlugin::receiptsPrinterOptions()
{
	QString printer_options_key = ReceiptsPlugin::SETTINGS_PREFIX;
	printer_options_key += "receiptsPrinter";
	qf::core::utils::Settings settings;
	QString s = settings.value(printer_options_key).toString();
	return ReceiptsPrinterOptions::fromJson(s.toUtf8());
}

void ReceiptsPlugin::setReceiptsPrinterOptions(const ReceiptsPrinterOptions &opts)
{
	QString printer_options_key = ReceiptsPlugin::SETTINGS_PREFIX;
	printer_options_key += "receiptsPrinter";
	qf::core::utils::Settings settings;
	QByteArray ba = opts.toJson();
	QString s = QString::fromUtf8(ba);
	settings.setValue(printer_options_key, s);

	QF_SAFE_DELETE(m_receiptsPrinter)
}

ReceiptsPrinter *ReceiptsPlugin::receiptsPrinter()
{
	if(!m_receiptsPrinter) {
		m_receiptsPrinter = new ReceiptsPrinter(receiptsPrinterOptions(), this);
	}
	return m_receiptsPrinter;
}

QVariantMap ReceiptsPlugin::readCardTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QVariantMap ret;
	quickevent::core::si::ReadCard read_card = getPlugin<CardReaderPlugin>()->readCard(card_id);
	{
		qfu::TreeTable tt;
		tt.appendColumn("position", QVariant::Int);
		tt.appendColumn("code", QVariant::Int);
		tt.appendColumn("punchTimeMs", QVariant::Int);
		tt.appendColumn("stpTimeMs", QVariant::Int);
		tt.appendColumn("lapTimeMs", QVariant::Int);
 		QMapIterator<QString, QVariant> it(read_card);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		int position = 0;
		int start_time_ms = read_card.startTime();
		if(start_time_ms == 0xeeee)
			start_time_ms = read_card.checkTime();
		start_time_ms *= 1000;
		int prev_stp_time_ms = 0;
		for(auto v : read_card.punches()) {
			quickevent::core::si::ReadPunch punch(v.toMap());
			int punch_time_ms = punch.time() * 1000 + punch.msec();
			int stp_time_ms = quickevent::core::og::TimeMs::msecIntervalAM(start_time_ms, punch_time_ms);
			int ix = tt.appendRow();
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			++position;
			int code = punch.code();
			tt_row.setValue("position", position);
			tt_row.setValue("code", code);
			tt_row.setValue("punchTimeMs", punch_time_ms);
			tt_row.setValue("stpTimeMs", stp_time_ms);
			tt_row.setValue("lapTimeMs", stp_time_ms - prev_stp_time_ms);
			prev_stp_time_ms = stp_time_ms;
			tt.setRow(ix, tt_row);
		}
		{
			int ix = tt.appendRow();
			//int code = punch.code();
			//ttr.setValue("position", position);
			//ttr.setValue("code", code);
			int punch_time_ms = read_card.finishTime() * 1000 + read_card.finishTimeMs();
			int stp_time_ms = quickevent::core::og::TimeMs::msecIntervalAM(start_time_ms, punch_time_ms);
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			tt_row.setValue(QStringLiteral("punchTimeMs"), punch_time_ms);
			tt_row.setValue(QStringLiteral("stpTimeMs"), stp_time_ms);
			tt_row.setValue(QStringLiteral("lapTimeMs"), stp_time_ms - prev_stp_time_ms);
			tt.setRow(ix, tt_row);
		}
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("config", "ckey, cvalue, ctype")
					.from("config")
					.where("ckey LIKE 'event.%'");
			qf::core::sql::Query q;
			q.exec(qb.toString());
			while(q.next()) {
				QVariant v = qf::core::Utils::retypeStringValue(q.value("cvalue").toString(), q.value("ctype").toString());
				tt.setValue(q.value("ckey").toString(), v);
			}
		}
		tt.setValue("stageCount", getPlugin<EventPlugin>()->stageCount());
		tt.setValue("currentStageId", getPlugin<EventPlugin>()->currentStageId());
		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

QVariantMap ReceiptsPlugin::receiptTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QF_TIME_SCOPE("receiptTablesData()");
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	QVariantMap ret;
	quickevent::core::si::ReadCard read_card = getPlugin<CardReaderPlugin>()->readCard(card_id);
	quickevent::core::si::CheckedCard checked_card = getPlugin<CardReaderPlugin>()->checkCard(read_card);
	int run_id = checked_card.runId();
	bool is_card_lent = getPlugin<CardReaderPlugin>()->isCardLent(card_id, read_card.finishTime(), run_id);
	int current_stage_id = getPlugin<EventPlugin>()->stageIdForRun(run_id);
	int course_id = checked_card.courseId();
	int current_standings = 0;
	int competitors_finished = 0;
	int best_time = 0;
	QMap<int, int> best_laps; // position->time
	QMap<int, int> lap_stand; // position->standing in lap
	QMap<int, int> lap_stand_cummulative;  // position->cummulative standing after lap
	{
		qf::core::model::SqlTableModel model;
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "*")
				.select2("runs", "*")
				.select2("classes", "name")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.from("runs")
				.join("runs.competitorId", "competitors.id")
				.join("competitors.classId", "classes.id")
				.where("runs.id=" QF_IARG(run_id));
		if(is_relays) {
			qb.select2("relays", "*")
					.join("runs.relayId", "relays.id");
		}
		model.setQuery(qb.toString());
		model.reload();
		if(model.rowCount() == 1) {
			qf::core::sql::Query run;
			run.execThrow("SELECT runlaps.position, runlaps.code, runlaps.lapTimeMs FROM runlaps WHERE runId = " QF_IARG(run_id) " ORDER BY position");
			while(run.next()) {
				int pos = run.value("position").toInt();
				int lapTimeMs = run.value("lapTimeMs").toInt();
				{
					// best lap times and position/place/standing in lap
					qf::core::sql::QueryBuilder qb_laps;
					qb_laps.select("MIN(runlaps.lapTimeMs) AS minLapTimeMs, SUM(CASE WHEN runlaps.lapTimeMs < " QF_IARG(lapTimeMs) " THEN 1 ELSE 0 END) as standingLap");
					if(is_relays) {
						int leg = model.value(0, "runs.leg").toInt();
						int class_id = model.value(0, "relays.classId").toInt();
						qb_laps.from("relays")
							.join("relays.id", "runs.relayId", qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("relays.classId=" QF_IARG(class_id)
								" AND runs.leg=" QF_IARG(leg));
					}
					else {
						int class_id = model.value(0, "competitors.classId").toInt();
						qb_laps.from("competitors")
							.join("competitors.id", "runs.competitorId",qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("runs.stageId=" QF_IARG(current_stage_id)
								" AND competitors.classId=" QF_IARG(class_id));


					}
					qb_laps.join("runs.id", "runlaps.runId", qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runlaps.position > 0"
							" AND runlaps.lapTimeMs > 0"
							" AND runlaps.position=" QF_IARG(pos)
							" AND NOT runs.disqualified"
							" AND runs.isRunning"
							" AND NOT runs.misPunch")
						.groupBy("runlaps.position");
					qf::core::sql::Query q_laps;
					q_laps.execThrow(qb_laps.toString());
					if (q_laps.next()) {
						best_laps[pos] = q_laps.value("minLapTimeMs").toInt();
						lap_stand[pos] = q_laps.value("standingLap").toInt() + 1;
					}
				}
				{
					// current position/place/standing after lap
					qf::core::sql::Query q_run_curr_time;
					q_run_curr_time.execThrow("SELECT SUM(lapTimeMs) as currTime FROM runlaps WHERE position <= " QF_IARG(pos) " AND runId = " QF_IARG(run_id) " GROUP BY runId" );
					q_run_curr_time.next();
					int run_curr_time = q_run_curr_time.value("currTime").toInt();

					qf::core::sql::QueryBuilder qb_curr_times;
					qb_curr_times.select("runs.id, SUM(runlaps.lapTimeMs) as currTime");
					if(is_relays) {
						int leg = model.value(0, "runs.leg").toInt();
						int class_id = model.value(0, "relays.classId").toInt();
						qb_curr_times.from("relays")
							.join("relays.id", "runs.relayId", qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("relays.classId=" QF_IARG(class_id)
								" AND runs.leg=" QF_IARG(leg));
					}
					else {
						int class_id = model.value(0, "competitors.classId").toInt();
						qb_curr_times.from("competitors")
							.join("competitors.id", "runs.competitorId",qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("runs.stageId=" QF_IARG(current_stage_id)
								" AND competitors.classId=" QF_IARG(class_id));


					}
					qb_curr_times.join("runs.id", "runlaps.runId", qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runlaps.position > 0"
							" AND runlaps.lapTimeMs > 0"
							" AND runlaps.position<=" QF_IARG(pos)
							" AND NOT runs.disqualified"
							" AND runs.isRunning"
							" AND NOT runs.misPunch")
						.groupBy("runs.id");
					qf::core::sql::Query q_curr_time;
					q_curr_time.execThrow("SELECT COUNT(*) as currStanding FROM (" + qb_curr_times.toString() + ") WHERE currTime < " QF_IARG(run_curr_time));
					if (q_curr_time.next()) {
						lap_stand_cummulative[pos] = q_curr_time.value("currStanding").toInt() + 1;
					}
				}
			}
		}
		if(checked_card.isOk()) {
			// find current standings
			qf::core::sql::QueryBuilder qb;
			if(is_relays) {
				int leg = model.value(0, "runs.leg").toInt();
				int class_id = model.value(0, "relays.classId").toInt();
				qb.select2("runs", "timeMs")
						.select("runs.disqualified OR NOT runs.isRunning OR runs.misPunch AS dis")
						.from("relays")
						.joinRestricted("relays.id", "runs.relayId",
										"relays.classId=" QF_IARG(class_id)
										" AND runs.finishTimeMs > 0"
										" AND runs.leg=" QF_IARG(leg),
										qf::core::sql::QueryBuilder::INNER_JOIN)
						.orderBy("misPunch, disqualified, isRunning, runs.timeMs");
			}
			else {
				int class_id = model.value(0, "competitors.classId").toInt();
				qb.select2("runs", "timeMs")
						.select("runs.disqualified OR NOT runs.isRunning OR runs.misPunch AS dis")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(current_stage_id) " AND competitors.classId=" QF_IARG(class_id), qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runs.finishTimeMs > 0")
						.orderBy("misPunch, disqualified, isRunning, runs.timeMs");
			}
			//qfInfo() << qb.toString();
			auto q = qf::core::sql::Query::fromExec(qb.toString());
			while (q.next()) {
				bool dis = q.value("dis").toBool();
				int time = q.value("timeMs").toInt();
				if(!dis) {
					if(time <= checked_card.timeMs())
						current_standings++;
					if(best_time == 0 || time < best_time)
						best_time = time;
				}
				competitors_finished++;
			}
		}
		qfu::TreeTable tt = model.toTreeTable();
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("courses", "length, climb")
					.select("(SELECT COUNT(*) FROM coursecodes WHERE courseId=courses.id) AS controlCount")
					.from("courses")
					.where("courses.id=" QF_IARG(course_id));
			qf::core::sql::Query q;
			q.exec(qb.toString());
			if(q.next()) {
				QSqlRecord rec = q.record();
				for (int i = 0; i < rec.count(); ++i) {
					QString fld_name = rec.fieldName(i);
					tt.setValue(fld_name, rec.value(i));
				}
			}
		}
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("config", "ckey, cvalue, ctype")
					.from("config")
					.where("ckey LIKE 'event.%'");
			qf::core::sql::Query q;
			q.exec(qb.toString());
			while(q.next()) {
				QVariant v = qf::core::Utils::retypeStringValue(q.value("cvalue").toString(), q.value("ctype").toString());
				tt.setValue(q.value("ckey").toString(), v);
			}
		}
		tt.setValue("stageCount", getPlugin<EventPlugin>()->stageCount());
		tt.setValue("currentStageId", getPlugin<EventPlugin>()->currentStageId());
		qfDebug() << "competitor:\n" << tt.toString();
		ret["competitor"] = tt.toVariant();
	}
	{
		qfu::TreeTable tt;
		tt.appendColumn("position", QVariant::Int);
		tt.appendColumn("code", QVariant::String);
		tt.appendColumn("stpTimeMs", QVariant::Int);
		tt.appendColumn("lapTimeMs", QVariant::Int);
		tt.appendColumn("standLap", QVariant::String);
		tt.appendColumn("standCummulative", QVariant::String);
		tt.appendColumn("lossMs", QVariant::Int);
		tt.appendColumn("distance", QVariant::Int);
		QMapIterator<QString, QVariant> it(checked_card);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		tt.setValue("isOk", checked_card.isOk());
		int position = 0;
		for(auto v : checked_card.punches()) {
			quickevent::core::si::CheckedPunch punch(v.toMap());
			int ix = tt.appendRow();
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			++position;
			int code = punch.code();
			tt_row.setValue("position", position);
			tt_row.setValue("code", QString::number(code).rightJustified(3, ' '));
			tt_row.setValue("stpTimeMs", punch.stpTimeMs());
			tt_row.setValue("standLap", QString::number(lap_stand[position]).rightJustified(2, ' '));
			tt_row.setValue("standCummulative", QString::number(lap_stand_cummulative[position]).rightJustified(2, ' '));
			int lap = punch.lapTimeMs();
			tt_row.setValue("lapTimeMs", lap);
			int best_lap = best_laps.value(position);
			if(lap > 0 && best_lap > 0) {
				int loss = lap - best_lap;
				tt_row.setValue("lossMs", loss);
			}
			tt_row.setValue("distance", punch.distance());
			tt.setRow(ix, tt_row);
		}
		{
			QSet<int> correct_codes;
			for (int i = 0; i < checked_card.punchCount(); ++i) {
				correct_codes << checked_card.punchAt(i).code();
			}
			QVariantList xc;
			for (int i = 0; i < read_card.punchCount(); ++i) {
				int code = read_card.punchAt(i).code();
				if(!correct_codes.contains(code)) {
					xc.insert(xc.count(), QVariantList() << (i+1) << code);
				}
			}
			tt.setValue("extraCodes", xc);
		}
		tt.setValue("currentStandings", current_standings);
		tt.setValue("competitorsFinished", competitors_finished);
		tt.setValue("timeMs", checked_card.timeMs());
		tt.setValue("bestTime", best_time);
		tt.setValue("isCardLent", is_card_lent);

		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

void ReceiptsPlugin::previewCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "previewReceipe_classic, card id:" << card_id;
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setPersistentSettingsId("cardPreview");
	w->setWindowTitle(tr("Card"));
	w->setReport(qmlDir() + "/sicard.qml");
	QVariantMap dt = readCardTablesData(card_id);
	for(auto key : dt.keys())
		w->setTableData(key, dt.value(key));
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(fwk);
	dlg.setCentralWidget(w);
	dlg.exec();
}

void ReceiptsPlugin::previewReceipt(int card_id)
{
	previewReceipt(card_id, currentReceiptPath());
}

bool ReceiptsPlugin::printReceipt(int card_id)
{
	QF_TIME_SCOPE("ReceiptsPlugin::printReceipt()");
	try {
		return printReceipt(card_id, currentReceiptPath());
	}
	catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

bool ReceiptsPlugin::printCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QF_TIME_SCOPE("ReceiptsPlugin::printCard()");
	try {
		QVariantMap dt = readCardTablesData(card_id);
		return receiptsPrinter()->printReceipt(qmlDir() + "/sicard.qml", dt);
	}
	catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

bool ReceiptsPlugin::printError(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QF_TIME_SCOPE("ReceiptsPlugin::printError()");
	try {
		QVariantMap dt = readCardTablesData(card_id);
		return receiptsPrinter()->printReceipt(qmlDir() + "/error.qml", dt);
	}
		catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

void ReceiptsPlugin::previewReceipt(int card_id, const QString &receipt_path)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "previewReceipe_classic, card id:" << card_id;
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setPersistentSettingsId("cardPreview");
	w->setWindowTitle(tr("Receipt"));
	w->setReport(receipt_path);
	QVariantMap dt = receiptTablesData(card_id);
	for(auto key : dt.keys())
		w->setTableData(key, dt.value(key));
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(fwk);
	dlg.setCentralWidget(w);
	dlg.exec();
}

bool ReceiptsPlugin::printReceipt(int card_id, const QString &receipt_path)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QVariantMap dt = receiptTablesData(card_id);
	return receiptsPrinter()->printReceipt(receipt_path, dt);
}

bool ReceiptsPlugin::isAutoPrintEnabled()
{
	return qff::MainWindow::frameWork()->findChild<ReceiptsWidget *>("ReceiptsWidget")->isAutoPrintEnabled();
}

void ReceiptsPlugin::printOnAutoPrintEnabled(int card_id)
{
	if(isAutoPrintEnabled())
		printReceipt(card_id);
}


}
