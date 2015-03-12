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
			.select("lastName || " " || firstName AS name")
			.from("competitors")
			.where("competitors.id={{ID}}");
	setQueryBuilder(qb);
}

bool CompetitorDocument::saveData()
{
	RecordEditMode old_mode = mode();
	bool ret = Super::saveData();
	//Log.info("CompetitorDocument", saveData_qml, "ret:", ret, "old_mode", old_mode, "vs", DataDocument.ModeInsert, old_mode == DataDocument.ModeInsert);
	if(ret) {
		if(old_mode == DataDocument::ModeInsert) {
			// insert laps
			int competitor_id = dataId().toInt();
			qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
			EventPlugin *event_plugin = qobject_cast<EventPlugin*>(fwk->plugin("Event"));
			QF_ASSERT(event_plugin != nullptr, "invalid Event plugin type", return false);

			int stage_count = event_plugin->eventConfig()->stageCount();
			qf::core::sql::Query q(model()->connectionName());
			q.prepare("INSERT INTO laps (competitorId, stageId) VALUES (:competitorId, :stageId)");
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":competitorId", competitor_id);
				q.bindValue(":stageId", i + 1);
				if(!q.exec()) {
					qfError() << q.lastError().text();
					break;
				}
			}
		}
	}
	return ret;

}

