import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		ThisPartWidget{
			id: thisPart
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

	onInstalled:
	{
		
		//var act_file = FrameWork.menuBar.actionForPath('tools');
		//var act_competitors = act_file.addMenuBefore('classes', qsTr('Cla&sses'));
		//var act_print = act_competitors.addMenuInto('print', qsTr('&Print'));
		//act_print.addActionInto(actPrintAll);
		FrameWork.addPartWidget(thisPart, root.manifest.featureId);
		
		// calling menuBar before addPartWidget() causes app crash
		var a = thisPart.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a.addActionInto(actPrintAll);		
	}

}
