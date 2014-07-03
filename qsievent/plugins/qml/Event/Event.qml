import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0
import "qrc:/qf/core/js/stringext.js" as StringExt

QfObject {
	id: root

	property string currentEventName

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

	function openEvent(event_name)
	{
		console.debug(db);
		var q = db.query();
		if(!event_name) {
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
			event_name = InputDialogSingleton.getItem(null, qsTr('Query'), qsTr('Open event'), events, 0, false);
		}
		if(event_name) {
			if(q.exec("SET SCHEMA '" + event_name + "'")) {
				root.currentEventName = event_name;
			}
		}
	}

	function whenServerConnected()
	{
		console.debug("whenServerConnected");
		if(FrameWork.plugin("SqlDb").sqlServerConnected) {
			var core_feature = FrameWork.plugin("Core");
			var settings = core_feature.createSettings();
			settings.beginGroup("sql/connection");
			var event_name = settings.value('event');
			settings.destroy();
			openEvent(event_name);
		}
	}
}
