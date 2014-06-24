import qf.core 1.0
import qf.qmlwidgets 1.0

Part {
	id: root
	property string featureId
	property var dependsOnFeatureIds
	property string visualSlot
	property bool disabled: false

	function _Plugin_install()
	{
		_Part_install();
		Log.debug("Plugin feature:", featureId, "installed");
	}

	function install()
	{
		_Plugin_install();
	}

	function activatedChanged(on_off)
	{
		Log.debug("Plugin feature:", featureId, "activatedChanged to:", on_off);
	}
}
