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
		//var steps = 3;
		//FrameWork.showProgress(qsTr("Importing event"), 1, steps);

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

				for(var class_obj in data.Classes) {
					var class_id = parseInt(data.Classes[class_obj].ID);
					var class_name = data.Classes[class_obj].Name;
					Log.info("adding class id:", class_id, "name:", class_name);
					classDoc.loadForInsert();
					classDoc.setValue("id", class_id);
					classDoc.setValue("name", class_name);
					classDoc.save();
					//break;
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
				db.transaction();
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
