import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QtObject api: QtObject
	{
		property alias currentEventName: event.currentEventName
	}

	property QfObject internals: QfObject {
		property Plugin pluginSqlDb: FrameWork.plugin("SqlDb")
		SqlConnection {
			id: db
		}

		Event {
			id: event
		}
	}

	property list<Action> actions: [
		Action {
			id: actCreateEvent
			text: qsTr('Create &new event')
			shortcut: "Ctrl+N"
			enabled: internals.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				Log.info(text, "triggered");
				event.createEvent();
			}
		},
		Action {
			id: actOpenEvent
			text: qsTr('&Open event')
			shortcut: "Ctrl+O"
			enabled: internals.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				Log.info(text, "triggered");
				event.openEvent();
			}
		},
		Action {
			id: actImportEventOris
			text: qsTr('From &Oris')
			enabled: internals.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				event.importOris()
			}
		}
	]

	Component.onCompleted:
	{
		var quit = FrameWork.menuBar.actionForPath('file/quit');
		quit.addActionBefore(actCreateEvent);
		//quit.addSeparatorBefore();
		quit.addActionBefore(actOpenEvent);
		quit.addMenuBefore('importEvent', qsTr('&Import event'));
		quit.addSeparatorBefore();

		FrameWork.menuBar.actionForPath('file/importEvent').addActionInto(actImportEventOris);

		FrameWork.plugin('SqlDb').api.sqlServerConnectedChanged.connect(event.whenServerConnected);
		FrameWork.statusBar.eventName = Qt.binding(function() {return root.api.currentEventName;});

	}
}
