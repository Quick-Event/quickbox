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
		SqlDatabase {
			id: db
		}
	}

	function createEvent()
	{
		var event_name = InputDialogSingleton.getText(null, qsTr('Query'), qsTr('Enter new event name'), qsTr('new_event'));
		if(event_name) {
			Log.info('will create:', event_name);
			var create_script = dbSchema.createSqlScript({schemaName: event_name, driverName: db.driverName});
			//Log.info(create_script.join(';\n') + ';');
			db.transaction();
			var q = db.query();
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
			if(ok)
				db.commit();
			else
				db.rollback();
		}
	}

	function openEvent()
	{
		console.debug(db);
		//var db = Sql.database();
		var q = db.query();
		var qb = q.builder();
		qb.select('nspname').from('pg_catalog.pg_namespace  AS n')
			.where("nspname NOT LIKE 'pg\\_%'")
			.where("nspname NOT IN ('public', 'information_schema')")
			.orderBy('nspname');
		q.exec(qb);
		var events = [];
		while(q.next()) {
			events.push(q.value('nspname'));
		}
		var event_name = InputDialogSingleton.getItem(null, qsTr('Query'), qsTr('Open event'), events, 0, false);
		if(event_name) {
			q.exec("SET SCHEMA '" + event_name + "'");
		}
	}

}
