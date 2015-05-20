import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Core 1.0

CorePlugin {
	id: root
	property QtObject api: QtObject
	{
		property Crypt crypt: Crypt
		{
			Component.onCompleted: {
				initGenerator(16808, 11, 2147483647);
			}
		}

		function createSettings()
		{
			// it is better to create and destroy new Settings object for each particular use
			// because it handles situation when one doesn't endGroup properly (groups can be nested)
			return settingsComponent.createObject(null);
		}

		function downloadContent(url, callback)
		{
			Log.info("http get request:", url);
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
	}

	property list<Action> actions: [
		Action {
			id: actQuit
			oid: 'quit'
			text: qsTr('&Quit')
			onTriggered: {
				Log.info(text, "triggered");
				Qt.quit();
			}
		},
		Action {
			id: actLaunchSqlTool
			//oid: 'quit'
			text: qsTr('&SQL tool')
			onTriggered: {
				Log.info(text, "triggered");
				root.launchSqlTool();
			}
		}
	]

	property QfObject internals: QfObject
	{
		Component {
			id: settingsComponent
			Settings {}
		}
		NetworkAccessManager {
			id: networkAccessManager
		}
	}

	onInstalled:
	{
		Log.debug("debug log test, use console.debug() for qml file and line information");
		Log.info("info log test, use console.info() for qml file and line information");
		Log.warning("warn log test, use console.warn() for qml file and line information");
		//Log.error("error log test, use console.error() for qml file and line information");
		console.debug("Core log test");
		console.info("Core log test");
		console.warn("Core log test");
		//console.error("Core log test");
		console.debug("Core plugin installed");

		FrameWork.setPersistentSettingDomains("quickbox.org", "QuickBox");
		FrameWork.persistentSettingsId = "MainWindow";
		FrameWork.loadPersistentSettings();

		console.debug(FrameWork.menuBar);
		var act_file = FrameWork.menuBar.actionForPath('file');
		act_file.text = qsTr('&File');
		var act_file_import = act_file.addMenuInto("import", qsTr("&Import"));
		act_file.addActionInto(act_file_import);
		act_file.addSeparatorInto();
		act_file.addActionInto(actQuit);

		var tools = FrameWork.menuBar.actionForPath('tools');
		tools.text = qsTr('&Tools');
		tools.addActionInto(actLaunchSqlTool);
		tools.addMenuInto('pluginSettings', qsTr('&Plugin settings'));

		FrameWork.menuBar.actionForPath('view').text = qsTr('&View');
		FrameWork.menuBar.actionForPath('help').text = qsTr('&Help');
	}

}
