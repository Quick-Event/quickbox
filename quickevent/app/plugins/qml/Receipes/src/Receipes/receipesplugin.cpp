#include "receipesplugin.h"
#include "../receipespartwidget.h"

#include <Event/eventplugin.h>
#include <CardReader/cardreaderplugin.h>
#include <CardReader/checkedcard.h>

#include <qf/core/utils/treetable.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <QSqlRecord>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;

using namespace Receipes;

ReceipesPlugin::ReceipesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ReceipesPlugin::installed, this, &ReceipesPlugin::onInstalled);
}

void ReceipesPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	ReceipesPartWidget *pw = new ReceipesPartWidget(manifest()->featureId());
	framework->addPartWidget(pw);
}

CardReader::CardReaderPlugin *ReceipesPlugin::cardReaderPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

Event::EventPlugin *ReceipesPlugin::eventPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

QVariantMap ReceipesPlugin::receipeTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QVariantMap ret;
	CardReader::CheckedCard cc = cardReaderPlugin()->checkCard(card_id);
	int run_id = cc.runId();
	int course_id = cc.courseId();
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
		model.reload(qb.toString());
		if(model.rowCount() == 1) {
			int class_id = model.value(0, "competitors.classId").toInt();
			qf::core::sql::QueryBuilder qb;
			qb.select("runlaps", "position")
					.select("MIN(runlaps.lapTimeMs) AS minLapTimeMs")
					.from("runlaps")
					.joinRestricted("runlaps.runId", "runs.id", "runlaps.lapTimeMs > 0")
					.join("runs.competitorId", "competitors.id")
					.where("competitors.classId=" QF_IARG(class_id))
					.groupBy("runlaps.position")
					.orderBy("runlaps.position");
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
 		QMapIterator<QString, QVariant> it(cc);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		for(auto v : cc.punches()) {
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
		QVariantList mc;
		for(auto i : missing_codes.keys())
			mc.insert(mc.count(), QVariantList() << i << missing_codes.value(i));
		tt.setValue("missingCodes", mc);
		{
			Event::Stage stage = eventPlugin()->stage(eventPlugin()->currentStageId());
			QTime start00 = stage.startTime();
			if(start00.hour() >= 12)
				start00 = start00.addSecs(-12 * 60 *60);
			tt.setValue("stageStart", start00);
		}
		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

void ReceipesPlugin::previewReceipe(int card_id)
{
	QMetaObject::invokeMethod(this, "previewReceipeClassic", Qt::DirectConnection, Q_ARG(QVariant, card_id));
}


