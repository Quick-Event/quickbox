#include "classdocument.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>

namespace Classes {

ClassDocument::ClassDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "*")
			.from("classes")
			.where("classes.id={{ID}}");
	setQueryBuilder(qb);
}

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	return qobject_cast<Event::EventPlugin *>(plugin);
}

bool ClassDocument::saveData()
{
	RecordEditMode old_mode = mode();
	bool ret = Super::saveData();
	//Log.info("CompetitorDocument", saveData_qml, "ret:", ret, "old_mode", old_mode, "vs", DataDocument.ModeInsert, old_mode == DataDocument.ModeInsert);
	if(ret) {
		if(old_mode == DataDocument::ModeInsert) {
			// insert classdefs
			int class_id = dataId().toInt();
			//int si_id = value("competitors.siId").toInt();

			auto *event_plugin = eventPlugin();
			QF_ASSERT(event_plugin != nullptr, "invalid Event plugin type", return false);

			int stage_count = event_plugin->stageCount();
			qf::core::sql::Query q(model()->connectionName());
			q.prepare("INSERT INTO classdefs (classId, stageId) VALUES (:classId, :stageId)");
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":classId", class_id);
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

bool ClassDocument::dropData()
{
	bool ret = false;
	auto id = dataId();
	{
		qf::core::sql::Query q(model()->connectionName());
		q.prepare("DELETE FROM classdefs WHERE classId = :classId");
		q.bindValue(":classId", id);
		ret = q.exec();
		if(!ret)
			qfError() << q.lastError().text();
	}
	if(ret) {
		ret = Super::dropData();
	}
	return ret;
}

}
