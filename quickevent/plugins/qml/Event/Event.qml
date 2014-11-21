import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt

QfObject {
	id: root

	property string currentEventName

	property QfObject internals: QfObject
	{
		DbSchema {
			id: dbSchema
		}
		SqlConnection {
			id: db
		}
	}

	function createEvent()
	{
		var event_name = InputDialogSingleton.getText(null, qsTr('Query'), qsTr('Enter new event name'), qsTr('new_event'));
		if(event_name) {
			Log.info('will create:', event_name);
			var create_script = dbSchema.createSqlScript({schemaName: event_name, driverName: db.driverName});
			Log.info(create_script.join(';\n') + ';');
            var q = db.createQuery();
            Log.info("QUERY:", q ,typeof q, q===null)
            db.transaction();
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
        var q = db.createQuery();
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
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		if(event_name) {
			if(q.exec("SET SCHEMA '" + event_name + "'")) {
				var settings = FrameWork.plugin('Core').api.createSettings();
				settings.beginGroup("sql/connection");
				settings.setValue("event", event_name);
				settings.destroy();
				root.currentEventName = event_name;
			}
		}
	}

	function importOris()
	{
		var d = new Date;
		d.setMonth(d.getMonth() - 2);
		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEventList&sport=1&datefrom=' + d.toISOString().slice(0, 10);
		FrameWork.plugin("Core").api.downloadContent(url, function(get_ok, json_str)
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
                MessageBoxSingleton.critical(FrameWork, "http get error: " + json_str + ' on: ' + url)
			}

		});
	}

	function importEventOris(event_id)
	{
		//var steps = 3;
		//FrameWork.showProgress(qsTr("Importing event"), 1, steps);

		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=' + event_id;
		FrameWork.plugin("Core").api.downloadContent(url, function(get_ok, json_str)
		{
			//Log.info("http get finished:", get_ok, url);
			if(get_ok) {
                var data = JSON.parse(json_str).Data;
				var stage_count = data.Stages;
				Log.info("pocet stage:", stage_count);
				var event_name = createEvent();
				if(!event_name)
					return;
				openEvent(event_name);

                var q = db.createQuery();

				q.prepare('INSERT INTO event (stageCount, name, description, date, place, mainReferee, director, importId) VALUES (:stageCount, :name, :description, :date, :place, :mainReferee, :director, :importId)');
				q.bindValue(':stageCount', stage_count);
				q.bindValue(':name', data.Name);
				q.bindValue(':description', '');
				q.bindValue(':date', data.Date);
				q.bindValue(':place', data.Place);
				q.bindValue(':mainReferee', data.MainReferee.FirstName + ' ' + data.MainReferee.LastName);
				q.bindValue(':director', data.Director.FirstName + ' ' + data.Director.LastName);
				q.bindValue(':importId', event_id);
				q.exec();


				q.prepare('INSERT INTO stages (id) VALUES (:id)');
				for(var i=0; i<stage_count; i++) {
					q.bindValue(':id', i+1);
					if(!q.exec())
						break;
				}

				// import classes
				q.prepare('INSERT INTO classes (id, name) VALUES (:id, :name)');
				for(var class_obj in data.Classes) {
					var class_name = data.Classes[class_obj].Name;
					Log.info("adding class:", class_name);
					q.bindValue(':id', class_name);
					q.bindValue(':name', class_name);
					if(!q.exec())
						break;
				}

				importEventOrisRunners(event_id, stage_count)
			}
			else {
				MessageBoxSingleton.critical("http get error: " + json_str + ' on: ' + url)
			}

		});
	}

	function importEventOrisRunners(event_id, stage_count)
	{
		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEventEntries&eventid=' + event_id;
		FrameWork.plugin("Core").api.downloadContent(url, function(get_ok, json_str)
		{
			//Log.info("http get finished:", get_ok, url);
			if(get_ok) {
				var data = JSON.parse(json_str).Data;
				// import competitors
				//FrameWork.showProgress(qsTr("Importing competitors"), 2, steps);
                var q = db.createQuery();
				q.prepare('INSERT INTO competitors (classId, siId, firstName, lastName, registration, licence, note, importId) VALUES (:classId, :siId, :firstName, :lastName, :registration, :licence, :note, :importId)');
                var q2 = db.createQuery();
				q2.prepare('INSERT INTO laps (stageId, competitorId, siId) VALUES (:stageId, :competitorId, :siId)');
				for(var competitor_obj_key in data) {
					var competitor_obj = data[competitor_obj_key];
					Log.debug(JSON.stringify(competitor_obj, null, 2));
					Log.info(competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName);
					var siid = parseInt(competitor_obj.SI);
					var note = competitor_obj.Note;
					if(isNaN(siid)) {
						note += ' SI:' + competitor_obj.SI;
						siid = 0;
					}
					if(competitor_obj.RequestedStart) {
						note += ' req. start: ' + competitor_obj.RequestedStart;
					}
					q.bindValue(':classId', competitor_obj.ClassDesc);
					q.bindValue(':siId', siid);
					q.bindValue(':firstName', competitor_obj.FirstName);
					q.bindValue(':lastName', competitor_obj.LastName);
					q.bindValue(':registration', competitor_obj.RegNo);
					q.bindValue(':licence', competitor_obj.Licence);
					q.bindValue(':note', note);
					q.bindValue(':importId', competitor_obj.ID);
					if(!q.exec()) {
						MessageBoxSingleton.critical(FrameWork, "SQL error: " + q.lastError())
						break;
					}
					var competitor_id = q.lastInsertedValue();
					for(var i=0; i<stage_count; i++) {
						q2.bindValue(':siId', siid);
						q2.bindValue(':competitorId', competitor_id);
						q2.bindValue(':stageId', i+1);
						if(!q2.exec()) {
							MessageBoxSingleton.critical(FrameWork, "SQL error: " + q2.lastError())
							break;							
						}
					}
				}
			}
			else {
				MessageBoxSingleton.critical(FrameWork, "http get error: " + json_str + ' on: ' + url)
			}
		});
	}


	function whenServerConnected()
	{
		console.debug("whenServerConnected");
		if(FrameWork.plugin("SqlDb").api.sqlServerConnected) {
			var core_feature = FrameWork.plugin("Core");
			var settings = core_feature.api.createSettings();
			settings.beginGroup("sql/connection");
			var event_name = settings.value('event');
			settings.destroy();
			openEvent(event_name);
		}
	}
}
