import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
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
				ok = q.exec(cmd);
				if(!ok) {
					Log.info(cmd);
					console.error(q.lastError());
					break;
				}
			}
			if(ok)
				db.commit();
			else {
				db.rollback();
				event_name = '';
			}
		}
		return event_name;
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

	function importOris()
	{
		var d = new Date;
		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEventList&sport=1&datefrom=' + d.toISOString().slice(0, 10);
		FrameWork.plugin("Core").downloadContent(url, function(get_ok, json_str)
		{
			//Log.info("http get finished:", get_ok, url);
			if(get_ok) {
				var json = JSON.parse(json_str)
				var event_descriptions = []
				var event_ids = []
				for(var event_name in json.Data) {
					var event = json.Data[event_name];
					//Log.info(event.Name)
					event_ids.push(event.ID);
					event_descriptions.push(event.Date + " " + event.Name + (event.Org1? (" " + event.Org1.Abbr): ""));
				}
				var ix = InputDialogSingleton.getItemIndex(null, qsTr('Query'), qsTr('Import event'), event_descriptions, 0, false);
				if(ix >= 0) {
					importEventOris(event_ids[ix]);
				}
			}
			else {
				MessageBoxSingleton.critical("http get error: " + json_str + ' on: ' + url)
			}

		});
	}

	function importEventOris(event_id)
	{
		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=' + event_id;
		FrameWork.plugin("Core").downloadContent(url, function(get_ok, json_str)
		{
			//Log.info("http get finished:", get_ok, url);
			if(get_ok) {
				var data = JSON.parse(json_str).Data;
				Log.info("pocet etap:", data.Stages);
				var event_name = createEvent();
				if(!event_name)
					return;
				for(var class_obj in data.Classes) {
					Log.info("class:", data.Classes[class_obj].Name);
				}
			}
			else {
				MessageBoxSingleton.critical("http get error: " + json_str + ' on: ' + url)
			}

		});
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
