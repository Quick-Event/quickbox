import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property list<Action> actions: [
		Action {
			id: actQuit
			oid: 'quit'
			text: qsTr('&Quit')
			onTriggered: {
				Log.info(text, "triggered");
				Qt.quit();
			}
		}
	]

	function createSettings()
	{
		// it is better to create and destroy new Settings object for each particular use
		// because it handles situation when one doesn't endGroup properly (groups can be nested)
		return settingsComponent.createObject(null);
	}

	function downloadContent(url, callback)
	{
		var reply = networkAccessManager.get(url);
		reply.downloadProgress.connect(FrameWork.showProgress);
		reply.finished.connect(function(get_ok) {
			Log.info("http get finished:", get_ok, reply.url);
			if(get_ok) {
				callback(true, reply.textData);
			}
			else {
				console.error("http get error:", reply.errorString, 'on:', reply.url);
				callback(false, reply.errorString);
			}
			reply.destroy();
		});
	}

	property Crypt crypt: Crypt
	{
		Component.onCompleted: {
			initGenerator(16808, 11, 2147483647);
		}
	}

	property QfObject internals: QfObject
	{
		Component {
			id: settingsComponent
			Settings {}
		}
		AppStatusBar {
			id: appStatusBar
		}
		NetworkAccessManager {
			id: networkAccessManager
		}
	}

	Component.onCompleted:
	{
		Log.debug("debug log test, use console.debug() for qml file and line information");
		Log.info("info log test, use console.info() for qml file and line information");
		Log.warning("warn log test, use console.warn() for qml file and line information");
		//Log.error("error log test, use console.error() for qml file and line information");
		console.debug("Core log test");
		console.info("Core log test");
		console.warn("Core log test");
		//console.error("Core log test");

		FrameWork.setPersistentSettingDomains("datamines.cz", "DataMines");
		FrameWork.persistentSettingsId = "MainWindow";
		FrameWork.loadPersistentSettings();

		console.debug(FrameWork.menuBar);
		FrameWork.menuBar.actionForPath('file').text = qsTr('&File');
		//FrameWork.menuBar.actionForPath('file').addSeparator();
		FrameWork.menuBar.actionForPath('file').addAction(actQuit);
		FrameWork.menuBar.actionForPath('view').text = qsTr('&View');
		FrameWork.menuBar.actionForPath('help').text = qsTr('&Help');

		FrameWork.setStatusBar(appStatusBar);
	}
}
