#include "classdocument.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>
#include <plugins/Event/src/eventplugin.h>

using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;

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

			int stage_count = getPlugin<EventPlugin>()->stageCount();
			qf::core::sql::Query q(sqlModel()->connectionName());
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
		qf::core::sql::Query q(sqlModel()->connectionName());
		q.prepare("DELETE FROM classdefs WHERE classId = :classId");
		q.bindValue(":classId", id);
		q.exec(qf::core::Exception::Throw);
		//if(!ret)
		//	qfError() << q.lastError().text();
		ret = Super::dropData();
	}
	return ret;
}

}
