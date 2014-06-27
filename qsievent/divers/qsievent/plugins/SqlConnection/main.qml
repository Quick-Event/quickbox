import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'SqlConnection'
	dependsOnFeatureIds: ["DbUtils"]

	actions: []

	property QfObject internals: QfObject
	{
	}

	function install()
	{
		FrameWork.postInstall.connect(postInstall);
	}

	function postInstall()
	{
		var dbf = FrameWork.plugin('DbUtils');
		dbf.connectToSqlServer(false);
	}

}
