import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'Event'
	dependsOnFeatureIds: ["Sql"]

	property QfObject internals: QfObject {
		property bool installFinished: false
		function isSqlConnected()
		{
			if(!installFinished)
				return false;
			return Sql.database().isOpen;
		}

		Event {
			id: event
		}
	}

	actions: [
		Action {
			id: actCreateEvent
			text: qsTr('Create &new event')
			enabled: internals.isSqlConnected()
			onTriggered: {
				Log.info(text, "triggered");
				event.createEvent();
			}
		},
		Action {
			id: actOpenEvent
			text: qsTr('&Open event')
			shortcut: "Ctrl+O"
			enabled: internals.isSqlConnected()
			onTriggered: {
				Log.info(text, "triggered");
				event.openEvent();
			}
		}
	]

	function install()
	{
		var quit = FrameWork.menuBar.actionForPath('file/quit', false);
		quit.prependAction(actCreateEvent);
		//quit.prependSeparator();
		quit.prependAction(actOpenEvent);
		quit.prependSeparator();
		internals.installFinished = true;
	}
}
