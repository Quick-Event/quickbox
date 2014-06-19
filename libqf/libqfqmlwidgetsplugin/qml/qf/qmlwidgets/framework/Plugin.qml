import qf.core 1.0
import qf.qmlwidgets 1.0

Part {
	id: root
	property string featureId
	property var dependsOnFeatureIds
	property string visualSlot
	property bool disabled: false

	function _Plugin_install(frame_work)
	{
		_Part_install(frame_work);
		Log.debug("Plugin feature:", featureId, "installed");
	}

	function install(frame_work)
	{
		_Plugin_install(frame_work);
	}

	function activatedChanged(on_off)
	{
		Log.debug("Plugin feature:", featureId, "activatedChanged to:", on_off);
	}
}
