#include "receiptsplugin.h"
#include "../receiptspartwidget.h"
#include "../receiptsprinter.h"

#include <Event/eventplugin.h>
#include <CardReader/cardreaderplugin.h>
#include <CardReader/checkedcard.h>

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

#include <QDomDocument>
#include <QSqlRecord>
#include <QPrinterInfo>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;

namespace Receipts {

const QLatin1String ReceiptsPlugin::SETTINGS_PREFIX("plugins/Receipts");

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

	QF_SAFE_DELETE(m_receiptsPrinter);
}

ReceiptsPrinter *ReceiptsPlugin::receiptsPrinter()
{
	if(!m_receiptsPrinter) {
		m_receiptsPrinter = new ReceiptsPrinter(receiptsPrinterOptions(), this);
	}
	return m_receiptsPrinter;
}

QVariantMap ReceiptsPlugin::receiptTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QF_TIME_SCOPE("receiptTablesData()");
	QVariantMap ret;
	CardReader::CheckedCard checked_card = cardReaderPlugin()->checkCard(card_id);
	int current_stage_id = eventPlugin()->currentStageId();
	int run_id = checked_card.runId();
	int course_id = checked_card.courseId();
	int current_standings = 1;
	int competitors_finished = 0;
	QMap<int, int> best_laps; //< code->time
	QMap<int, int> missing_codes; //< pos->code
	QSet<int> out_of_order_codes;
	{
		// load all codes as missing ones
		qf::core::sql::QueryBuilder qb;
		qb.select2("coursecodes", "position")
				.select2("codes", "code, outOfOrder")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(course_id))
				//.where("NOT codes.outOfOrder")
				.orderBy("coursecodes.position");
		qf::core::sql::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while (q.next()) {
			int pos = q.value("position").toInt();
			int code = q.value("code").toInt();
			bool ooo = q.value("outOfOrder").toBool();
			//class_codes << code;
			missing_codes[pos] = code;
			if(ooo)
				out_of_order_codes << code;
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
				qf::core::sql::QueryBuilder qb_minlaps;
				// TODO: remove position field from DB in 0.1.5
				qb_minlaps.select("runlaps.position, MIN(runlaps.lapTimeMs) AS minLapTimeMs")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(current_stage_id) " AND competitors.classId=" QF_IARG(class_id), "JOIN")
						.joinRestricted("runs.id", "runlaps.runId", "runlaps.position > 0 AND runlaps.lapTimeMs > 0", "JOIN")
						.groupBy("runlaps.position");
				QString qs = qb_minlaps.toString();
				qfInfo() << qs;
				qf::core::sql::Query q;
				q.exec(qs);
				while(q.next()) {
					int position = q.value("position").toInt();
					if(position == 0) {
						qfWarning() << "position == 0 in best runlaps";
						continue;
					}
					int lap = q.value("minLapTimeMs").toInt();
					if(lap == 0) {
						qfWarning() << "minLapTimeMs == 0 in best runlaps";
						continue;
					}
					best_laps[position] = lap;
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
			int position = punch.position();
			int code = punch.code();
			if(position > 0) {
				missing_codes.remove(position);
			}
			ttr.setValue("position", position);
			ttr.setValue("code", code);
			ttr.setValue("stpTimeMs", punch.stpTimeMs());
			int lap = punch.lapTimeMs();
			ttr.setValue("lapTimeMs", lap);
			int best_lap = best_laps.value(position);
			if(lap > 0 && best_lap > 0) {
				int loss = lap - best_lap;
				ttr.setValue("lossMs", loss);
			}
		}
		{
			// runlaps table contains also finish time entry, it is under FINISH_PUNCH_POS
			// currently best_laps[999] contains best finish lap time for this class
			int loss = 0;
			int best_lap = best_laps.value(CardReader::CardReaderPlugin::FINISH_PUNCH_POS);
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
		tt.setValue("currentStandings", current_standings);
		tt.setValue("competitorsFinished", competitors_finished);
		//tt.setValue("cardNumber", checked_card.cardNumber());

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

bool ReceiptsPlugin::printReceipt(int card_id)
{
	QF_TIME_SCOPE("ReceiptsPlugin::printReceipt()");
	try {
		printReceipt_classic(card_id);
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

void ReceiptsPlugin::printReceipt_classic(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QVariantMap dt = receiptTablesData(card_id);
	receiptsPrinter()->printReceipt(manifest()->homeDir() + "/reports/receiptClassic.qml", dt);
}

}
