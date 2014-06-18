import qf.core 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	featureId: 'Core'
	property var visualSlots: ['runners', 'start', 'finish']

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		frameWork.menuOnPath('event').title = qsTr('Event');
		frameWork.menuOnPath('help').title = qsTr('Help');

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
