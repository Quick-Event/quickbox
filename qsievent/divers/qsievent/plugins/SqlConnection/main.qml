import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'SqlConnection'
	dependsOnFeatureIds: ["DbUtils"]

	signal installed();

	actions: []

	property QfObject internals: QfObject
	{
	}

	function postInstall()
	{
		Log.info("postInstall queued connection on SqlConnection")
	}

}
