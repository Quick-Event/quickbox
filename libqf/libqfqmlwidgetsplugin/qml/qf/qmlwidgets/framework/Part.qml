import QtQuick 2.0

import qf.core 1.0
import qf.qmlwidgets 1.0

QtObject {
	id: root
	property string oid
	property var framework
	property list<Action> actions

	function _Part_install(_framework)
	{
		Log.debug("Part oid:", oid, "installed");
		framework = _framework
	}

	function install(_framework)
	{
		_Part_install(_framework);
	}

	function activatedChanged(on_off)
	{
		Log.debug("Part oid:", oid, "activatedChanged to:", on_off);
	}
}