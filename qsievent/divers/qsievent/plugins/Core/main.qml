import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root

	actions: [
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

	property QfObject internals: QfObject
	{
		Component {
			id: settingsComponent
			Settings {}
		}
	}

	function createSettings()
	{
		return settingsComponent.createObject(null);
	}

	property Crypt crypt: Crypt
	{
		Component.onCompleted: {
			initGenerator(16808, 11, 2147483647);
		}
	}

	Component.onCompleted:
	{
		Log.debug("debug log test, use console.debug() for qml file and line information");
		Log.info("info log test, use console.info() for qml file and line information");
		Log.warning("warn log test, use console.warn() for qml file and line information");
		Log.error("error log test, use console.error() for qml file and line information");
		console.debug("Core log test");
		console.info("Core log test");
		console.warn("Core log test");
		console.error("Core log test");

		FrameWork.setPersistentSettingDomains("datamines.cz", "DataMines");
		FrameWork.persistentSettingsId = "MainWindow";
		FrameWork.loadPersistentSettings();

		console.debug(FrameWork.menuBar);
		FrameWork.menuBar.actionForPath('file').text = qsTr('&File');
		//FrameWork.menuBar.actionForPath('file').addSeparator();
		FrameWork.menuBar.actionForPath('file').addAction(actQuit);
		FrameWork.menuBar.actionForPath('help').text = qsTr('&Help');
	}
}
