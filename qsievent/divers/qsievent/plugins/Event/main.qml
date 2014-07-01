import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root

	property QfObject internals: QfObject {
		property Plugin sqlDb: FrameWork.plugin('SqlDb')

		Event {
			id: event
		}
	}

	actions: [
		Action {
			id: actCreateEvent
			text: qsTr('Create &new event')
			shortcut: "Ctrl+N"
			enabled: internals.sqlDb.database.isOpen
			onTriggered: {
				Log.info(text, "triggered");
				event.createEvent();
			}
		},
		Action {
			id: actOpenEvent
			text: qsTr('&Open event')
			shortcut: "Ctrl+O"
			enabled: internals.sqlDb.database.isOpen
			onTriggered: {
				Log.info(text, "triggered");
				event.openEvent();
			}
		}
	]

	Component.onCompleted:
	{
		var quit = FrameWork.menuBar.actionForPath('file/quit', false);
		quit.prependAction(actCreateEvent);
		//quit.prependSeparator();
		quit.prependAction(actOpenEvent);
		quit.prependSeparator();
	}
}
