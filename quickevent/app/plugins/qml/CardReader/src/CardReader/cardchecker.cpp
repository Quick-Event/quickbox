#include "cardchecker.h"
#include "../CardReader/cardreaderplugin.h"

#include <Event/eventplugin.h>
#include <Event/stage.h>
#include <Runs/runsplugin.h>

#include <siut/simessage.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/assert.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>

#include <QTime>

namespace qfs = qf::core::sql;

using namespace CardReader;

CardChecker::CardChecker(QObject *parent)
	: QObject(parent)
{

}

int CardChecker::fixTimeWrapAM(int time1_msec, int time2_msec)
{
	constexpr int hr12ms = 12 * 60 * 60 * 1000;
	while(time2_msec < time1_msec)
		time2_msec += hr12ms;
	return time2_msec;
}

int CardChecker::msecIntervalAM(int time1_msec, int time2_msec)
{
	return fixTimeWrapAM(time1_msec, time2_msec) - time1_msec;
}

int CardChecker::toAMms(int time_msec)
{
	return SIMessageCardReadOut::toAMms(time_msec);
}

int CardChecker::toAM(int time_sec)
{
	return SIMessageCardReadOut::toAM(time_sec);
}

int CardChecker::stageStartSec()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto event_plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	int ret = event_plugin->stageStart(event_plugin->currentStageId());
	return ret / 1000;
}

int CardChecker::startTimeSec(int run_id)
{
	int ret = 0;
	qfs::QueryBuilder qb;
	qb.select2("runs", "startTimeMs")
			.from("runs")
			.where("runs.id=" QF_IARG(run_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	if(q.next())
		ret = q.value(0).toInt() / 1000;
	else
		qfError() << "Cannot find runs record for id:" << run_id;
	return ret;
}

QVariantMap CardChecker::courseCodesForRunId(int run_id)
{
	QVariantMap ret;
	if(run_id <= 0) {
		qfError() << "Run ID == 0";
		return ret;
	}
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto runs_plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT(runs_plugin != nullptr, "Bad plugin", return QVariantMap());

	int course_id = runs_plugin->courseForRun(run_id);
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
	{
		qfs::QueryBuilder qb;
		qb.select2("coursecodes", "position")
				.select2("codes", "code, outOfOrder")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(course_id))
				.orderBy("coursecodes.position");
		qfs::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		QVariantList codes;
		while (q.next()) {
			codes << q.values();
		}
		ret["codes"] = codes;
	}
	return ret;
}

int CardChecker::finishPunchCode()
{
	return CardReader::CardReaderPlugin::FINISH_PUNCH_CODE;
}

