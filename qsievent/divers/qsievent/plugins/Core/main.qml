import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'Core'

	property var visualSlots: ['runners', 'start', 'finish']

	actions: [
		Action {
			id: actLayoutTest
			text: qsTr('&Layout test')
			shortcut: "Ctrl+T"
			onTriggered: {
				Log.info(text, "triggered");
				var dlg = Qt.createQmlObject("DlgLayoutTest {}", FrameWork);
                dlg.exec();
                dlg.destroy();
			}
		}
	]

	function settings()
	{
		return Qt.createQmlObject("import qf.core 1.0; Settings {}", root);
	}

	function install()
	{
		//_Plugin_install();
		FrameWork.setPersistentSettingDomains("datamines.cz", "DataMines");
		FrameWork.persistentSettingsId = "MainWindow";
		FrameWork.loadPersistentSettings();
		console.debug(FrameWork.menuBar);
		FrameWork.menuBar.itemForPath('file').title = qsTr('&File');
		FrameWork.menuBar.itemForPath('help').title = qsTr('Help');
		FrameWork.menuBar.itemForPath('help').addAction(actLayoutTest);

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
