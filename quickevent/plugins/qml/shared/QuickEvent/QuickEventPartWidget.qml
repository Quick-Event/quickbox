import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

PartWidget
{
	id: root

	Component.onCompleted:
	{
		FrameWork.plugin("SqlDb").api.onSqlServerConnectedChanged.connect(reloadIfActive);
		FrameWork.plugin("Event").api.onCurrentEventNameChanged.connect(reloadIfActive);
	}

	function canActivate(active_on)
	{
		console.debug(title, "canActivate:", active_on);
		reload_helper(active_on);
		return true;
	}

	function reloadIfActive()
	{
		reload_helper(root.active);
	}

	function reload_helper(active_on)
	{
		if(active_on) {
			var sql_connected = FrameWork.plugin("SqlDb").api.sqlServerConnected;
			var event_name = FrameWork.plugin("Event").api.currentEventName;
			if(sql_connected && event_name)
				reload();
		}
	}

	function pluginHomeDir()
	{
		var plugin = FrameWork.pluginForObject(root);
		return plugin.homeDir();
	}

}
