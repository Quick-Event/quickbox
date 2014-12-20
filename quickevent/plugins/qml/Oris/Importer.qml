import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt
import Event 1.0
import Classes 1.0
import Competitors 1.0

QtObject {
	id: root

	property QfObject internals: QfObject
	{
		SqlConnection {
			id: db
		}
		CompetitorDocument {
			id: competitorDoc
		}
		ClassDocument {
			id: classDoc
		}
	}
	/*
	function chooseAndImport2()
	{
		db.transaction();
		var q = db.createQuery();
		q.exec("DELETE FROM classes");
		for(var i=0; i<10; i++) {
			var class_id = i + 10 * 1;
			var class_name = "K" + class_id;
			Log.info("adding class id:", class_id, "name:", class_name);
			var use_doc = true;
			if(use_doc) {
				classDoc.loadForInsert();
				//classDoc.setValue("id", class_id);
				classDoc.setValue("name", class_name);
				classDoc.save();
			}
			else {
				var qs = "INSERT INTO classes (\"id\", \"name\") VALUES (:class_id, :class_name)"
				q.prepare(qs);
				q.bindValue(':class_id', class_id);
				q.bindValue(':class_name', class_name);
				var ok = q.exec();
				if(!ok) {
					console.error(q.lastError());
					break;
				}
			}
		}
		db.commit();
	}
	*/
	function chooseAndImport()
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
					importEvent(event_ids[ix]);
				}
			}
			else {
                MessageBoxSingleton.critical(FrameWork, "http get error: " + json_str + ' on: ' + url)
			}

		});
	}

	function importEvent(event_id)
	{
		var url = 'http://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=' + event_id;
		FrameWork.plugin("Core").api.downloadContent(url, function(get_ok, json_str)
		{
			//Log.info("http get finished:", get_ok, url);
			if(get_ok) {
				//Log.info("Imported event:", json_str);
				var event_api = FrameWork.plugin("Event").api;
                var data = JSON.parse(json_str).Data;
				var stage_count = parseInt(data.Stages);
				if(!stage_count)
					stage_count = 1;
				Log.info("pocet etap:", stage_count);
				var event_name = event_api.createEvent(stage_count);
				if(!event_name)
					return;

				event_api.openEvent(event_name);

                var cfg = event_api.config;
				cfg.setValue('event.stageCount', stage_count);
				cfg.setValue('event.name', data.Name);
				cfg.setValue('event.description', '');
				cfg.setValue('event.date', data.Date);
				cfg.setValue('event.place', data.Place);
				cfg.setValue('event.mainReferee', data.MainReferee.FirstName + ' ' + data.MainReferee.LastName);
				cfg.setValue('event.director', data.Director.FirstName + ' ' + data.Director.LastName);
				cfg.setValue('event.importId', event_id);
				cfg.save();

				db.transaction();
				var items_processed = 0;
				var items_count = 0;
				for(var class_obj in data.Classes) {
					items_count++;
				}
				for(var class_obj in data.Classes) {
					var class_id = parseInt(data.Classes[class_obj].ID);
					var class_name = data.Classes[class_obj].Name;
					FrameWork.showProgress("Importing class: " + class_name, items_processed++, items_count);
					Log.info("adding class id:", class_id, "name:", class_name);
					classDoc.loadForInsert();
					classDoc.setValue("id", class_id);
					classDoc.setValue("name", class_name);
					classDoc.save();
					//break;
				}
				db.commit();

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
				var competitors_processed = 0;
				var competitors_count = 0;
				for(var competitor_obj_key in data) {
					competitors_count++;
				}
				db.transaction();
				for(var competitor_obj_key in data) {
					var competitor_obj = data[competitor_obj_key];
					Log.debug(JSON.stringify(competitor_obj, null, 2));
					Log.info(competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName, "classId:", parseInt(competitor_obj.ClassID));
					var siid = parseInt(competitor_obj.SI);
					var note = competitor_obj.Note;
					if(isNaN(siid)) {
						note += ' SI:' + competitor_obj.SI;
						siid = 0;
					}
					if(competitor_obj.RequestedStart) {
						note += ' req. start: ' + competitor_obj.RequestedStart;
					}
					FrameWork.showProgress("Importing: " + competitor_obj.LastName + " " + competitor_obj.FirstName, competitors_processed, competitors_count);
					competitorDoc.loadForInsert();
					competitorDoc.setValue('classId', parseInt(competitor_obj.ClassID));
					competitorDoc.setValue('siId', siid);
					competitorDoc.setValue('firstName', competitor_obj.FirstName);
					competitorDoc.setValue('lastName', competitor_obj.LastName);
					competitorDoc.setValue('registration', competitor_obj.RegNo);
					competitorDoc.setValue('licence', competitor_obj.Licence);
					competitorDoc.setValue('note', note);
					competitorDoc.setValue('importId', competitor_obj.ID);
					competitorDoc.save();
					//break;
					competitors_processed++;
				}
				db.commit();
				FrameWork.plugin("Event").api.reloadActivePart();
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
