import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Competitors 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0

CompetitorsPlugin {
	id: root

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
			id: actPrintCompetitorsStatistics
			text: qsTr('Competitors statistics')
			onTriggered: {
				statistics.printCompetitorsStatistics()
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", false);
		a.addActionInto(actPrintCompetitorsStatistics);
	}

}
