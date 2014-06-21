import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	property var dbSchema: DbSchema {}
	featureId: 'DbUtils'
    //dependsOnFeatureIds: "Core"

	actions: [
		Action {
			id: actConnectDb
			text: qsTr('&Connect to databse')
			shortcut: "Ctrl+T"
			onTriggered: {
				Log.info(text, "triggered");
				//var dlg = QmlWidgets.createWidget("Dialog", root.frameWork);
				var dlg = Qt.createQmlObject("DlgConnectDb {}", root.frameWork);
                dlg.exec();
			}
		},
		Action {
			id: actCreateEvent
			text: qsTr('Create new event')
			onTriggered: {
				Log.info(text, "triggered");
			}
		},
		Action {
			id: actQuit
			text: qsTr('&Quit')
			onTriggered: {
				Log.info(text, "triggered");
				Qt.quit();
			}
		}
	]

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		frameWork.menuBar.itemForPath('file').addAction(actConnectDb);
		frameWork.menuBar.itemForPath('file').addSeparator();
		frameWork.menuBar.itemForPath('file').addAction(actCreateEvent);
		frameWork.menuBar.itemForPath('file').addSeparator();
		frameWork.menuBar.itemForPath('file').addAction(actQuit);
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
