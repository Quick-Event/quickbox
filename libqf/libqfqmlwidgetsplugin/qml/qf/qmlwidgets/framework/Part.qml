import QtQml 2.0

import qf.core 1.0
import qf.qmlwidgets 1.0

QtObject {
	id: root
	property list<Action> actions

	//protected properties
	property  var frameWork

	function _Part_install(frame_work)
	{
		frameWork = frame_work
		//Log.debug("Part oid:", oid, "installed");
	}

	function install(frame_work)
	{
		_Part_install(frame_work);
	}

}
