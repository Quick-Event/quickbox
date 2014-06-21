import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	featureId: 'Core'
	property var visualSlots: ['runners', 'start', 'finish']

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		Application.setOrganizationDomain("datamines.cz");
		Application.setOrganizationName("DataMines");
		console.debug(frameWork.menuBar);
		frameWork.menuBar.itemForPath('file').title = qsTr('&File');
		frameWork.menuBar.itemForPath('help').title = qsTr('Help');

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
