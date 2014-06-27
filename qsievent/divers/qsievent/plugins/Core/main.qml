import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'Core'

	property var visualSlots: ['runners', 'start', 'finish']

	property QfObject internals: QfObject
	{
    	Component {
    		id: settingsComponent
        	Settings {}
    	}
	}

	function settings()
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
		FrameWork.menuBar.itemForPath('file').title = qsTr('&File');
		FrameWork.menuBar.itemForPath('help').title = qsTr('Help');

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
