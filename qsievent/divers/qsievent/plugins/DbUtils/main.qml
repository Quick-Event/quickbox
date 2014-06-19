import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	featureId: 'DbUtils'

	actions: Action {
		id: actCreateEvent
		text: qsTr('Create new event')
		onTriggered: {
			Log.info(text, "triggered");
		}
	}

	function install(frame_work)
	{
		_Plugin_install(frame_work);
		frameWork.menuBar.ensureMenuOnPath('file').addAction(actCreateEvent);
		//framework.addMenu('tools', actCreateEvent);
	}
}
