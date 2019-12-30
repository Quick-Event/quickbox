#include "cardchecker.h"
#include "cardreaderplugin.h"

//#include <Classes/classesplugin.h>
#include <Event/eventplugin.h>
#include <Event/stage.h>
#include <Runs/runsplugin.h>

#include <quickevent/core/og/timems.h>
#include <quickevent/core/codedef.h>
#include <quickevent/core/si/punchrecord.h>

//#include <siut/simessage.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/assert.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>

#include <QTime>

namespace qfs = qf::core::sql;

namespace CardReader {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	return fwk->plugin<Event::EventPlugin*>();
}

static Runs::RunsPlugin* runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	return fwk->plugin<Runs::RunsPlugin*>();
}

CardChecker::CardChecker(QObject *parent)
	: QObject(parent)
{
}

int CardChecker::fixTimeWrapAM(int time1_msec, int time2_msec)
{
	return quickevent::core::og::TimeMs::fixTimeWrapAM(time1_msec, time2_msec);
}

int CardChecker::msecIntervalAM(int time1_msec, int time2_msec)
{
	return quickevent::core::og::TimeMs::msecIntervalAM(time1_msec, time2_msec);
}

int CardChecker::toAMms(int time_msec)
{
	return fixTimeWrapAM(0, time_msec);
	//return SIMessageCardReadOut::toAMms(time_msec);
}

int CardChecker::toAM(int time_sec)
{
	return toAMms(time_sec * 1000) / 1000;
	//return SIMessageCardReadOut::toAM(time_sec);
}

int CardChecker::stageIdForRun(int run_id)
{
	return eventPlugin()->stageIdForRun(run_id);
}

int CardChecker::stageStartSec(int stage_id)
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto event_plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	if(stage_id == 0)
		stage_id = event_plugin->currentStageId();
	int ret = event_plugin->stageStartMsec(stage_id);
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

int CardChecker::cardCheckCheckTimeSec()
{
	return eventPlugin()->eventConfig()->cardCheckCheckTimeSec();
}

QVariantMap CardChecker::courseCodesForRunId(int run_id)
{
	return runsPlugin()->courseCodesForRunId(run_id, true);
}

}
