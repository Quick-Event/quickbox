#include "competitordocument.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>

using namespace Competitors;

CompetitorDocument::CompetitorDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select("lastName || ' ' || firstName AS name")
			//.select2("classes", "name AS className")
			.from("competitors")
			//.join("competitors.classId", "classes.id")
			.where("competitors.id={{ID}}");
	setQueryBuilder(qb);
}

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	return qobject_cast<Event::EventPlugin *>(plugin);
}

bool CompetitorDocument::saveData()
{
	qfLogFuncFrame();
	RecordEditMode old_mode = mode();
	bool siid_dirty = isDirty("competitors.siId");
	bool class_dirty = isDirty("competitors.classId");
	//int old_siid = value("competitors.siId").toInt();
	bool ret = Super::saveData();
	qfDebug() << "Super save data:" << ret;
	if(ret) {
		if(old_mode == DataDocument::ModeInsert) {
			// insert runs
			qfDebug() << "inserting runs";
			int competitor_id = dataId().toInt();
			int si_id = value("competitors.siId").toInt();
			auto *event_plugin = eventPlugin();
			QF_ASSERT(event_plugin != nullptr, "invalid Event plugin type", return false);

			int stage_count = event_plugin->stageCount();
			qf::core::sql::Query q(model()->connectionName());
			q.prepare("INSERT INTO runs (competitorId, stageId, siId) VALUES (:competitorId, :stageId, :siId)");
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":competitorId", competitor_id);
				q.bindValue(":stageId", i + 1);
				if(si_id > 0 && m_saveSiidToRuns)
					q.bindValue(":siId", si_id);
				q.exec(qf::core::Exception::Throw);
			}
			eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
		}
		else if(old_mode == DataDocument::ModeEdit) {
			if(siid_dirty) {
				qfDebug() << "updating SIID in run tables";
				int si_id = value("competitors.siId").toInt();
				if(si_id > 0 && m_saveSiidToRuns) {
					int competitor_id = dataId().toInt();
					qf::core::sql::Query q(model()->connectionName());
					q.prepare("UPDATE runs SET siId=:siId WHERE competitorId=:competitorId", qf::core::Exception::Throw);
					q.bindValue(":competitorId", competitor_id);
					q.bindValue(":siId", si_id);
					q.exec(qf::core::Exception::Throw);
				}
			}
			if(class_dirty)
				eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
		}
	}
	return ret;
}

bool CompetitorDocument::dropData()
{
	bool ret = false;
	auto id = dataId();
	{
		qf::core::sql::Query q(model()->connectionName());
		q.prepare("DELETE FROM runs WHERE competitorId = :competitorId");
		q.bindValue(":competitorId", id);
		ret = q.exec();
		if(!ret)
			qfError() << q.lastError().text();
	}
	if(ret) {
		ret = Super::dropData();
		eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	}
	return ret;
}

bool CompetitorDocument::isSaveSiidToRuns() const
{
	return m_saveSiidToRuns;
}

void CompetitorDocument::setSaveSiidToRuns(bool save_siid_to_runs)
{
	m_saveSiidToRuns = save_siid_to_runs;
}

