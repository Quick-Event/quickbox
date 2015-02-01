import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		property ThisPartWidget thisPart
		Component {
			id: cThisPartWidget
			ThisPartWidget {
			}
		}
	}

	property list<Action> actions: [
		Action {
			id: actPrintAll
			text: qsTr('All')
			onTriggered: {
				internals.thisPart.printAll()
			}
		}
	]

	Component.onCompleted:
	{
		var act_file = FrameWork.menuBar.actionForPath('file');
		var act_competitors = act_file.addMenuAfter('competitors', qsTr('&Competitors'));
		var act_print = act_competitors.addMenuInto('print', qsTr('&Print'));
		act_print.addActionInto(actPrintAll);
		internals.thisPart = cThisPartWidget.createObject(FrameWork);
		FrameWork.addPartWidget(internals.thisPart, root.manifest.featureId);
	}

}
