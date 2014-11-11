import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		Component {
			id: dlgLayoutTest
			DlgLayoutTest {}
		}
	}

	property list<Action> actions: [
		Action {
			id: actLAboutQt
			text: qsTr('About &Qt')
			onTriggered: {
				MessageBoxSingleton.aboutQt();
			}
		},
		Action {
			id: actLayoutTest
			text: qsTr('&Layout test')
			onTriggered: {
				Log.info(text, "triggered");
				var dlg = dlgLayoutTest.createObject(FrameWork);
				dlg.exec();
				dlg.destroy();
			}
		},
		Action {
			id: actGC
			text: qsTr('Collect garbage')
			onTriggered: {
				Log.info(text, "triggered");
				gc();
			}
		}
	]

	Component.onCompleted:
	{
		FrameWork.menuBar.actionForPath('help').addAction(actLAboutQt);
		FrameWork.menuBar.actionForPath('help').addSeparator();
		FrameWork.menuBar.actionForPath('help').addAction(actLayoutTest);
		FrameWork.menuBar.actionForPath('help').addAction(actGC);
	}
}
