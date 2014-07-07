import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	//property alias currentEventName: event.currentEventName

	property QfObject internals: QfObject {
		property Plugin pluginSqlDb: FrameWork.plugin("SqlDb")
		SqlDatabase {
			id: db
		}

		Event {
			id: event
		}
		NetworkAccessManager {
			id: networkAccessManager
		}
	}

	property list<Action> actions: [
		Action {
			id: actCreateEvent
			text: qsTr('Create &new event')
			shortcut: "Ctrl+N"
			enabled: internals.pluginSqlDb.sqlServerConnected
			onTriggered: {
				Log.info(text, "triggered");
				event.createEvent();
			}
		},
		Action {
			id: actOpenEvent
			text: qsTr('&Open event')
			shortcut: "Ctrl+O"
			enabled: internals.pluginSqlDb.sqlServerConnected
			onTriggered: {
				Log.info(text, "triggered");
				event.openEvent();
			}
		},
		Action {
			id: actImportEventOris
			text: qsTr('From &Oris')
			enabled: internals.pluginSqlDb.sqlServerConnected
			onTriggered: {
				var d = new Date;
				//var reply = networkAccessManager.get('http://oris.orientacnisporty.cz/API/?format=json&method=getEventList&sport=1&datefrom=' + d.toISOString().slice(0, 10));
				var reply = networkAccessManager.get('http://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=2526');
				reply.downloadProgress.connect(FrameWork.showProgress);
				reply.finished.connect(function(get_ok) {
					Log.info("http get finished:", get_ok, reply.url);
					if(get_ok) {
						var json_str = reply.textData;
						var json = JSON.parse(json_str)
						Log.info("json:", json);
						json_str = JSON.stringify(json, null, 2)
						Log.info("text:", json_str);
					}
					else {
						console.error("http get error:", reply.errorString, 'on:', reply.url)
					}
					reply.destroy();
				});
			}
		}
	]

	Component.onCompleted:
	{
		var quit = FrameWork.menuBar.actionForPath('file/quit', false);
		quit.prependAction(actCreateEvent);
		//quit.prependSeparator();
		quit.prependAction(actOpenEvent);
		quit.prependMenu('importEvent', qsTr('&Import event'));
		quit.prependSeparator();

		FrameWork.menuBar.actionForPath('file/importEvent').addAction(actImportEventOris);

		FrameWork.plugin('SqlDb').sqlServerConnectedChanged.connect(event.whenServerConnected);
		FrameWork.statusBar.eventName = Qt.binding(function() {return event.currentEventName;});

	}
}
