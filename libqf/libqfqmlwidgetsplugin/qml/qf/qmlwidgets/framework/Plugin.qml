import qf.core 1.0
import qf.qmlwidgets.framework 1.0

Part {
	id: root
	property var manifest: null

	function activatedChanged(on_off)
	{
		Log.debug("Plugin feature:", featureId, "activatedChanged to:", on_off);
	}
}
