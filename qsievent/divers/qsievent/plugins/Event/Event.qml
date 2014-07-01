import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0
import "qrc:/qf/core/js/stringext.js" as StringExt

QfObject {
	id: root

	property QfObject internals: QfObject
	{
		DbSchema {
			id: dbSchema
		}
	}

	function createEvent()
	{
		var event_name = InputDialogSingleton.getText(null, qsTr('Query'), qsTr('Enter new event name'), qsTr('new_event'));
		if(event_name) {
			Log.info('will create:', event_name);
			var create_script = dbSchema.createSqlScript({schemaName: event_name});
			//Log.info(create_script.join(';\n') + ';');
			var db = Sql.database();
			db.transaction();
			var q = db.createQuery();
			var ok = false;
			for(var i=0; i<create_script.length; i++) {
				var cmd = create_script[i] + ';';
				if(cmd.startsWith('--'))
					continue;
				console.debug(cmd);
				ok = q.exec(cmd);
				if(!ok) {
					console.error(q.lastError());
					break;
				}
			}
			q.destroy();
			if(ok)
				db.commit();
			else
				db.rollback();
		}
	}

	function openEvent()
	{
		var q = Sql.database().exec("SELECT * FROM pg_user");
		var rec = q.record();
		var fld_names = rec.fieldNames().join(',');
		Log.info(fld_names);
		while(q.next()) {
			Log.info(q.values().join(','));
		}
	}

}
