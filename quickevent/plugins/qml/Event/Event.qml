import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt
import Competitors 1.0

QtObject {
	id: root

	property string currentEventName

	property QfObject internal: QfObject
	{
		DbSchema {
			id: dbSchema
		}
		SqlConnection {
			id: db
		}
		CompetitorDocument {
			id: compDoc
		}
		EventDocument {
			id: eventDoc
		}
		Component {
			id: cDlgCreateEvent
			DlgCreateEvent {}
		}
		function eventNameToFileName(event_name)
		{
			var event_fn = null;
			if(event_name) {
				var settings = FrameWork.plugin('Core').api.createSettings();
				settings.beginGroup("sql/singleFile");
				event_fn = settings.value("workingDir") + "/" + event_name + ".qbe";
				settings.destroy();
			}
			return event_fn;
		}
	}

	function createEvent(stage_count)
	{
		var event_name = ""
		console.debug("createEvent()", "stage_count:", stage_count);
		var dlg = cDlgCreateEvent.createObject(FrameWork);
		if(stage_count > 0) {
			dlg.stageCountReadOnly = true;
			dlg.stageCount = stage_count;
		}
		if(dlg.exec()) {
			event_name = dlg.eventName;
			stage_count = dlg.stageCount;
			Log.info('will create:', event_name);
			var sqldb_api = FrameWork.plugin("SqlDb").api;
			var connection_type = sqldb_api.connectionType;

			var db_open = db.isOpen;
			if(connection_type == "singleFile") {
				var event_fn = root.internal.eventNameToFileName(event_name);
				db.close();
				db.databaseName = event_fn;
				db_open = db.open();
			}
			if(db_open) {
				var create_options = {schemaName: event_name, driverName: db.driverName};
				//if(db.driverName.endsWith("SQLITE")) {
				//	create_options.schemaName = "main";
				//}
				var create_script = dbSchema.createSqlScript(create_options);
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
				if(ok) {
					console.debug('creating stages:', stage_count);
					var stage_table_name = 'stages';
					if(connection_type == "sqlServer")
						stage_table_name = event_name + '.' + stage_table_name;
					q.prepare('INSERT INTO ' + stage_table_name + ' (id) VALUES (:id)');
					for(var i=0; i<stage_count; i++) {
						q.bindValue(':id', i+1);
						if(!q.exec()) {
							ok = false;
							break;
						}
					}
				}
				if(ok) {
					db.commit();
				}
				else {
					db.rollback();
					MessageBoxSingleton.critical(FrameWork, qsTr("Create Database Error: %1").arg(q.lastError()));
					event_name = '';
				}
			}
		}
		dlg.destroy();

		return event_name;
	}

	function openEvent(event_name, connection_type)
	{
		var sqldb_api = FrameWork.plugin("SqlDb").api;
		if(!connection_type)
			connection_type = sqldb_api.connectionType;

		console.debug("openEvent()", "event_name:", event_name, "connection_type:", connection_type);
		var ok = false;
		if(connection_type == "sqlServer") {
			console.debug(db);
			var q = db.createQuery();
			var qb = q.createBuilder();
			if(!event_name) {
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
					settings.beginGroup("sql/" + connection_type);
					settings.setValue("event", event_name);
					settings.destroy();
					ok = true;
				}
			}
		}
		else if(connection_type == "singleFile") {
			var event_fn = null;
			if(event_name) {
				event_fn = root.internal.eventNameToFileName(event_name);
			}
			else {
				var settings = FrameWork.plugin('Core').api.createSettings();
				settings.beginGroup("sql/" + connection_type);

				event_fn = InputDialogSingleton.getOpenFileName(root, qsTr("Select event"), settings.value("workingDir"), qsTr("Quick Event files (*.qbe)"));
				if(event_fn) {
					event_fn = event_fn.replace("\\", "/");
					var ix = event_fn.lastIndexOf("/");
					event_name = event_fn.substring(ix + 1, event_fn.length - 4);
					var working_dir = event_fn.substring(0, ix);
					if(event_name) {
						settings.setValue("event", event_name);
						settings.setValue("workingDir", working_dir);
					}
				}
				settings.destroy();
			}
			//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
			if(event_fn) {
				db.close();
				db.databaseName = event_fn;
				Log.info("Opening database file", event_fn);
				if(db.open()) {
					var q = db.createQuery();
					q.exec("PRAGMA foreign_keys=ON");
					q.exec("pragma short_column_names=0;");
					ok = q.exec("pragma full_column_names=1;");
					Log.info("setting depricated pragma full column names:", ok);
				}
				else {
					MessageBoxSingleton.critical(FrameWork, qsTr("Open Database Error: %1").arg(db.errorString()));
				}
			}
		}
		else {
			console.error("Invalid connection type:", connection_type)
		}
		if(ok) {
			root.currentEventName = event_name;
			FrameWork.plugin('Event').api.config.reload();
		}
		return ok;
	}

	function whenServerConnected()
	{
		console.debug("whenServerConnected");
		var sqldb_api = FrameWork.plugin("SqlDb").api;
		if(sqldb_api.sqlServerConnected) {
			var core_feature = FrameWork.plugin("Core");
			var settings = core_feature.api.createSettings();
			settings.beginGroup("sql/" + sqldb_api.connectionType);
			var event_name = settings.value('event');
			settings.destroy();
			openEvent(event_name, sqldb_api.connectionType);
		}
	}
}
