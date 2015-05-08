#include "competitordocument.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>

CompetitorDocument::CompetitorDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select("lastName || ' ' || firstName AS name")
			.from("competitors")
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
	RecordEditMode old_mode = mode();
	bool ret = Super::saveData();
	//Log.info("CompetitorDocument", saveData_qml, "ret:", ret, "old_mode", old_mode, "vs", DataDocument.ModeInsert, old_mode == DataDocument.ModeInsert);
	if(ret) {
		if(old_mode == DataDocument::ModeInsert) {
			// insert runs
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
				if(si_id > 0)
					q.bindValue(":siId", si_id);
				if(!q.exec()) {
					qfError() << q.lastError().text();
					//break; can be succesfull in other stages
				}
			}
		}
		else if(old_mode == DataDocument::ModeEdit) {
			int si_id = value("competitors.siId").toInt();
			if(si_id > 0) {
				int competitor_id = dataId().toInt();
				qf::core::sql::Query q(model()->connectionName());
				q.prepare("UPDATE runs SET siId=:siId WHERE competitorId=:competitorId AND siId IS NULL", qf::core::Exception::Throw);
				q.bindValue(":competitorId", competitor_id);
				q.bindValue(":siId", si_id);
				if(!q.exec()) {
					qfError() << q.lastError().text();
				}
			}
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
	}
	return ret;
}

