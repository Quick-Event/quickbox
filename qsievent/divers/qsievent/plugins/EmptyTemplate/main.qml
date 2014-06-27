import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	featureId: 'EmptyTemplate'
	dependsOnFeatureIds: ["Core"]

	property QfObject internals: QfObject
	{
		Component {
			id: dlgLayoutTest
			DlgLayoutTest {}
		}
	}

	actions: [
		Action {
			id: actLayoutTest
			text: qsTr('&Layout test')
			shortcut: "Ctrl+T"
			onTriggered: {
				Log.info(text, "triggered");
				var dlg = dlgLayoutTest.createObject(FrameWork);
				dlg.exec();
				dlg.destroy();
			}
		},
		Action {
			id: actGC
			text: qsTr('Collect grbage')
			onTriggered: {
				Log.info(text, "triggered");
				gc();
			}
		}
	]

	function install()
	{
		FrameWork.menuBar.itemForPath('help').addAction(actLayoutTest);
		FrameWork.menuBar.itemForPath('help').addAction(actGC);
	}
}
