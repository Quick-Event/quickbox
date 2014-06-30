import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'Core'

	//property var visualSlots: ['runners', 'start', 'finish']

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

	function install()
	{
    	Log.debug("debug log test");
    	Log.info("info log test");
    	Log.warning("warn log test");
    	Log.error("error log test");
//_Plugin_install();
		FrameWork.setPersistentSettingDomains("datamines.cz", "DataMines");
		FrameWork.persistentSettingsId = "MainWindow";
		FrameWork.loadPersistentSettings();

		console.debug(FrameWork.menuBar);
		FrameWork.menuBar.actionForPath('file').text = qsTr('&File');
		//FrameWork.menuBar.actionForPath('file').addSeparator();
		FrameWork.menuBar.actionForPath('file').addAction(actQuit);
		FrameWork.menuBar.actionForPath('help').text = qsTr('Help');

		/*
		try {
			_Plugin_install(frame_work);
		}
		catch(err) {
			Log.error("install error", err);
		}
		*/
	}
}
