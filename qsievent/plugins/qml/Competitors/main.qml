import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		ThisPartWidget{
			id: thisPart
		}
	}

	/*
	property list<Action> actions: [
		Action {
			id: actLAboutQt
			text: qsTr('About &Qt')
			onTriggered: {
				MessageBoxSingleton.aboutQt();
			}
		}
	]
		*/

	Component.onCompleted:
	{
		//FrameWork.menuBar.actionForPath('help').addAction(actLAboutQt);
		//FrameWork.menuBar.actionForPath('help').addSeparator();
		FrameWork.addPartWidget(thisPart);
	}
}
