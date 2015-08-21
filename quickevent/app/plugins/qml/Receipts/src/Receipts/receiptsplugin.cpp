#include "receiptsplugin.h"
#include "../receiptspartwidget.h"

#include <Event/eventplugin.h>
#include <CardReader/cardreaderplugin.h>
#include <CardReader/checkedcard.h>

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

#include <QSqlRecord>
#include <QPrinterInfo>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;

using namespace Receipts;

ReceiptsPlugin::ReceiptsPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ReceiptsPlugin::installed, this, &ReceiptsPlugin::onInstalled);
}

void ReceiptsPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	ReceiptsPartWidget *pw = new ReceiptsPartWidget(manifest()->featureId());
	framework->addPartWidget(pw);
}

CardReader::CardReaderPlugin *ReceiptsPlugin::cardReaderPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

Event::EventPlugin *ReceiptsPlugin::eventPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

QVariantMap ReceiptsPlugin::receiptTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QVariantMap ret;
	CardReader::CheckedCard checked_card = cardReaderPlugin()->checkCard(card_id);
	int current_stage_id = eventPlugin()->currentStageId();
	int run_id = checked_card.runId();
	int course_id = checked_card.courseId();
	int current_standings = 1;
	int competitors_finished = 0;
	int control_count = 0;
	QMap<int, int> best_laps;
	QMap<int, int> missing_codes;
	{
		// load all codes as missing ones
		qf::core::sql::QueryBuilder qb;
		qb.select2("coursecodes", "position")
				.select2("codes", "code, outOfOrder")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(course_id))
				.where("NOT codes.outOfOrder")
				.orderBy("coursecodes.position");
		qf::core::sql::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while (q.next()) {
			missing_codes[q.value("position").toInt()] = q.value("code").toInt();
			control_count++;
		}
	}
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
		model.setQuery(qb.toString());
		model.reload();
		if(model.rowCount() == 1) {
			int class_id = model.value(0, "competitors.classId").toInt();
			{
				// find best laps for competitors class
				qf::core::sql::QueryBuilder qb;
				qb.select2("runlaps", "position")
						.select("MIN(runlaps.lapTimeMs) AS minLapTimeMs")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(current_stage_id) " AND competitors.classId=" QF_IARG(class_id))
						.joinRestricted("runs.id", "runlaps.runId", "runlaps.position > 0")
						.where("runlaps.lapTimeMs > 0")
						.groupBy("runlaps.position")
						.orderBy("runlaps.position");
				//qfInfo() << qb.toString();
				qf::core::sql::Query q;
				q.exec(qb.toString());
				while(q.next()) {
					int pos = q.value("position").toInt();
					if(pos == 0) {
						qfWarning() << "position == 0 in best runlaps";
						continue;
					}
					int lap = q.value("minLapTimeMs").toInt();
					if(lap == 0) {
						qfWarning() << "minLapTimeMs == 0 in best runlaps";
						continue;
					}
					best_laps[pos] = lap;
					//qfInfo() << "bestlaps[" << pos << "] =" << lap;
				}
			}
			{
				// find current standings
				qf::core::sql::QueryBuilder qb;
				qb.select2("runs", "timeMs")
						.select("runs.disqualified OR runs.offRace OR runs.misPunch AS dis")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(current_stage_id) " AND competitors.classId=" QF_IARG(class_id))
						.where("runs.finishTimeMs > 0")
						.orderBy("misPunch, disqualified, offRace, runs.timeMs");
				qf::core::sql::Query q;
				q.exec(qb.toString(), qf::core::Exception::Throw);
				while (q.next()) {
					bool dis = q.value("dis").toBool();
					int time = q.value("timeMs").toInt();
					if(!dis) {
						if(time < checked_card.timeMs())
							current_standings++;
					}
					competitors_finished++;
				}
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
		tt.setValue("stageCount", eventPlugin()->stageCount());
		tt.setValue("currentStageId", eventPlugin()->currentStageId());
		qfDebug() << "competitor:\n" << tt.toString();
		ret["competitor"] = tt.toVariant();
	}
	{
		qfu::TreeTable tt;
		tt.appendColumn("position", QVariant::Int);
		tt.appendColumn("code", QVariant::Int);
		tt.appendColumn("stpTimeMs", QVariant::Int);
		tt.appendColumn("lapTimeMs", QVariant::Int);
		tt.appendColumn("lossMs", QVariant::Int);
 		QMapIterator<QString, QVariant> it(checked_card);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		for(auto v : checked_card.punches()) {
			CardReader::CheckedPunch punch(v.toMap());
			qfu::TreeTableRow ttr = tt.appendRow();
			int pos = punch.position();
			if(pos > 0)
				missing_codes.remove(pos);
			ttr.setValue("position", pos);
			ttr.setValue("code", punch.code());
			ttr.setValue("stpTimeMs", punch.stpTimeMs());
			int lap = punch.lapTimeMs();
			ttr.setValue("lapTimeMs", lap);
			int best_lap = best_laps.value(pos);
			if(best_lap > 0) {
				int loss = lap - best_lap;
				ttr.setValue("lossMs", loss);
			}
		}
		{
			// runlaps table contains also finish time entry, it is under last position
			// for exaple: if course is of 10 controls best_laps[10] contains best finish lap time for this class
			int loss = 0;
			int best_lap = best_laps.value(control_count + 1);
			if(best_lap > 0)
				loss = checked_card.finishLapTimeMs() - best_lap;
			//qfInfo() << "control_count:" << control_count << "finishLapTimeMs:" << checked_card.finishLapTimeMs() << "- best_lap:" << best_lap << "=" << loss;
			tt.setValue("finishLossMs", loss);
		}
		{
			QVariantList mc;
			for(auto i : missing_codes.keys())
				mc.insert(mc.count(), QVariantList() << i << missing_codes.value(i));
			//mc.insert(mc.count(), QVariantList() << 1 << 101);
			tt.setValue("missingCodes", mc);
		}
		{
			Event::StageData stage = eventPlugin()->stageData(eventPlugin()->currentStageId());
			QTime start00 = stage.startTime();
			if(start00.hour() >= 12)
				start00 = start00.addSecs(-12 * 60 *60);
			tt.setValue("stageStart", start00);
		}
		tt.setValue("currentStandings", current_standings);
		tt.setValue("competitorsFinished", competitors_finished);
		tt.setValue("cardNumber", checked_card.cardNumber());

		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

void ReceiptsPlugin::previewReceipt(int card_id)
{
	//QMetaObject::invokeMethod(this, "previewReceipeClassic", Qt::DirectConnection, Q_ARG(QVariant, card_id));
	previewReceipt_classic(card_id);
}

bool ReceiptsPlugin::printReceipt(int card_id, const QPrinterInfo &printer_info)
{
	try {
		printReceipt_classic(card_id, printer_info);
		return true;
	}
	catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

void ReceiptsPlugin::previewReceipt_classic(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "previewReceipe_classic, card id:" << card_id;
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setPersistentSettingsId("cardPreview");
	w->setWindowTitle(tr("Receipt"));
	w->setReport(manifest()->homeDir() + "/reports/receiptClassic.qml");
	QVariantMap dt = receiptTablesData(card_id);
	for(auto key : dt.keys())
		w->setTableData(key, dt.value(key));
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(fwk);
	dlg.setCentralWidget(w);
	dlg.exec();
}

void ReceiptsPlugin::printReceipt_classic(int card_id, const QPrinterInfo &printer_info)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "printReceipe_classic, card id:" << card_id;
	QPrinterInfo pi = printer_info;
	if(pi.isNull()) {
		for(auto s : QPrinterInfo::availablePrinterNames()) {
			qfInfo() << "available printer:" << s;
		}
		pi = QPrinterInfo::defaultPrinter();
	}
	if(pi.isNull()) {
		qfWarning() << "Default printer not set";
		return;
	}
	qfInfo() << "printing on:" << pi.printerName();
	QPrinter printer(pi);
	qf::qmlwidgets::reports::ReportProcessor rp(&printer);
	rp.setReport(manifest()->homeDir() + "/reports/receiptClassic.qml");
	QVariantMap dt = receiptTablesData(card_id);
	for(auto key : dt.keys()) {
		rp.setTableData(key, dt.value(key));
	}
	rp.process();
	qf::qmlwidgets::reports::ReportItemMetaPaintReport *doc = rp.processorOutput();
	qf::qmlwidgets::reports::ReportItemMetaPaint *it = doc->child(0);
	if(it) {
		qf::qmlwidgets::reports::ReportPainter painter(&printer);
		painter.drawMetaPaint(it);
	}
}
