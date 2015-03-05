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

	onInstalled:
	{
		internals.thisPart = cThisPartWidget.createObject(FrameWork);
		FrameWork.addPartWidget(internals.thisPart, root.manifest.featureId);

		// calling menuBar before addPartWidget() causes app crash
		var a = internals.thisPart.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a.addActionInto(actPrintAll);		
	}

}
