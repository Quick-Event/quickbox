import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

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
