import qf.core
import qf.widgets.framework

Part {
	oid: 'DbUtils'

	actions: Action {
		id: actCreateEvent
		caption: qsTr('Create event')
		onTriggered: {
			qf.core.Info(caption, "triggered");
		}
	}

	function install(framework)
	{
		_Part_install(framework);
		//framework.addMenu('tools', actCreateEvent);
	}
}