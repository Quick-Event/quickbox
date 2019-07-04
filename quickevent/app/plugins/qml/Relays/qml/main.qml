import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Relays 1.0
//import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
import "./"

RelaysPlugin {
	id: root
	/*
	property QfObject internals: QfObject
	{
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
		Statistics {
			id: statistics
			competitorsPlugin: root
		}
	}

	property list<Action> actions: [
		Action {
			id: actPrintRelaysStatistics
			text: qsTr('Relays statistics')
			onTriggered: {
				statistics.printRelaysStatistics()
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", false);
		a.addActionInto(actPrintRelaysStatistics);
	}
	*/
}
