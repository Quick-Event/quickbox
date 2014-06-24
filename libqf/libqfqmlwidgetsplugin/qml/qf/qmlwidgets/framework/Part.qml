import QtQml 2.0

import qf.core 1.0
import qf.qmlwidgets 1.0

QtObject {
	id: root
	property list<Action> actions

	function _Part_install()
	{
		//Log.debug("Part oid:", oid, "installed");
	}

	function install()
	{
		_Part_install();
	}

}
