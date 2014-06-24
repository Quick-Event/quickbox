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
				//var dlg = QmlWidgets.createWidget("Dialog", root.frameWork);
				var dlg = Qt.createQmlObject("DlgLayoutTest {}", root.frameWork);
                dlg.exec();
                dlg.destroy();
			}
		}
	]

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		Application.setApplicationNames("datamines.cz", "DataMines");
		frameWork.persistentSettingsId = "MainWindow";
		frameWork.loadPersistentSettings();
		console.debug(frameWork.menuBar);
		frameWork.menuBar.itemForPath('file').title = qsTr('&File');
		frameWork.menuBar.itemForPath('help').title = qsTr('Help');
		frameWork.menuBar.itemForPath('help').addAction(actLayoutTest);

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
