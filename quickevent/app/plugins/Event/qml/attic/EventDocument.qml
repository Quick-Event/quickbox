import QtQml 2.0
import qf.core 1.0

SqlDataDocument {
	id: root
	
	property QfObject internal: QfObject
	{
		SqlConnection {
			id: db
		}
	}

	Component.onCompleted:
	{
		queryBuilder.select2('event', '*')
			.from('event')
			.where('id={{ID}}');
	}

	function saveData_qml()
	{
		var old_mode = root.mode;
		if(root.saveData()) {
			if(old_mode == DataDocument.ModeInsert) {
				// insert stages
				var stage_count = root.value("stage")
				var q = db.createQuery();
				q.prepare('INSERT INTO stages (id) VALUES (:id)');
				for(var i=0; i<stage_count; i++) {
					q.bindValue(':id', i+1);
					if(!q.exec())
						break;
				}
			}
		}
	}
}