import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		ThisPartWidget{
			id: thisPart
			pluginId: root.manifest.featureId
		}
	}

	property list<Action> actions: [
		Action {
			id: actPrintAll
			text: qsTr('All')
			onTriggered: {
				thisPart.printAll()
			}
		}
	]

	Component.onCompleted:
	{
		var act_file = FrameWork.menuBar.actionForPath('file');
		var act_competitors = act_file.appendMenu('competitors', qsTr('&Competitors'));
		var act_print = act_competitors.addMenu('print', qsTr('&Print'));
		act_print.addAction(actPrintAll);
		FrameWork.addPartWidget(thisPart, root.manifest.featureId);
	}

}
