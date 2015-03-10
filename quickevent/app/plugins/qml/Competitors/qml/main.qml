import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Competitors 1.0

CompetitorsPlugin {
	id: root

	property QfObject internals: QfObject
	{
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

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a.addActionInto(actPrintAll);		
	}

}
