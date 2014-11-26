import QtQml 2.0
import qf.core 1.0

SqlDataDocument {
	id: root
	
	property QfObject internal: QfObject {

		SqlConnection {
			id: db
		}
	}

	Component.onCompleted:
	{
		queryBuilder.select2('competitors', '*')
			.select("lastName || ' ' || firstName AS name")
			.from('competitors').where('competitors.id={{ID}}');
	}

	function saveData_qml()
	{
		//Log.info("CompetitorDocument", saveData_qml);
		var old_mode = root.mode;
		var ret = root.saveData();
		//Log.info("CompetitorDocument", saveData_qml, "ret:", ret, "old_mode", old_mode, "vs", DataDocument.ModeInsert, old_mode == DataDocument.ModeInsert);
		if(ret) {
			if(old_mode == DataDocument.ModeInsert) {
				// insert laps
				var competitor_id = root.dataId;
				var stage_count = FrameWork.plugin('Event').api.config.getValue("event.stageCount", 1);
				//Log.info("CompetitorDocument", saveData_qml, "stage_count:", stage_count);
				var q = db.createQuery();
				q.prepare('INSERT INTO laps (competitorId) VALUES (:competitorId)');
				for(var i=0; i<stage_count; i++) {
					q.bindValue(':competitorId', competitor_id);
					if(!q.exec())
						break;
				}
			}
		}
		return ret;
	}
}