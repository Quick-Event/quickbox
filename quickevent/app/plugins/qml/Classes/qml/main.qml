import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Classes 1.0

ClassesPlugin {
	id: root

	property QfObject internals: QfObject
	{
	}
/*
	property list<Action> actions: [
		Action {
			id: actPrintAll
			text: qsTr('All')
			onTriggered: {
				//thisPart.printAll()
			}
		}
	]
*/
	onNativeInstalled:
	{
		//var a = root.partWidget.menuBar.actionForPath("print", true);
		//a.text = qsTr("&Print");
		//a.addActionInto(actPrintAll);
	}

}
