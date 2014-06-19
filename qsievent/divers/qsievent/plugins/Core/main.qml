import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	featureId: 'Core'
	property var visualSlots: ['runners', 'start', 'finish']

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		console.debug(frameWork.menuBar);
		frameWork.menuBar.ensureMenuOnPath('file').title = qsTr('File');
		frameWork.menuBar.ensureMenuOnPath('help').title = qsTr('Help');

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
