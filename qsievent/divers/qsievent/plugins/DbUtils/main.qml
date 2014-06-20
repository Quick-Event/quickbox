import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	property var dbSchema: DbSchema {}
	featureId: 'DbUtils'

	actions: Action {
		id: actCreateEvent
		text: qsTr('Create new event')
		onTriggered: {
			Log.info(text, "triggered");
		}
	}

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		frameWork.menuBar.itemForPath('file').addAction(actCreateEvent);
		//framework.addMenu('tools', actCreateEvent);
		/*
		var c = Qt.createComponent("DbSchema.qml");
        if (c.status == Component.Ready) {
            root.dbSchema = c.createObject(root);
        }
        else {
        	Log.error("Error creating DbSchema:", c.errorString());
        }
        Log.info(dbSchema);
        */
	}
}
