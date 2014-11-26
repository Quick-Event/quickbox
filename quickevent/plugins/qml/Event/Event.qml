import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt
import Competitors 1.0

QtObject {
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
		CompetitorDocument {
			id: compDoc
		}
		EventDocument {
			id: eventDoc
		}
	}

	function createEvent(stage_count)
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
			if(ok) {
				db.commit();
				console.debug('creating stages:', stage_count);
				q.prepare('INSERT INTO ' + event_name + '.stages (id) VALUES (:id)');
				for(var i=0; i<stage_count; i++) {
					q.bindValue(':id', i+1);
					if(!q.exec())
						break;
				}
			}
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
				settings.beginGroup("sql/connection");
				settings.setValue("event", event_name);
				settings.destroy();
				root.currentEventName = event_name;
				FrameWork.plugin('Event').api.config.reload();
			}
		}
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
