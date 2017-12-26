import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property list<Action> actions: [
		Action {
			id: actReportLentCards
			text: qsTr('&Competitors with card lent')
			onTriggered: {
				printCompetitorsWithCardLent();
			}
		}
	]

	onInstalled:
	{
		//console.warn("Oris installed");
		var act = FrameWork.menuBar.actionForPath('help/Examples/Reports');
		act.addActionInto(actReportLentCards)
	}

	function printCompetitorsWithCardLent()
	{
		Log.info("Example printCompetitorsWithCardLent triggered");
		QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/competitorsWithCardLent.qml" //report
									   , null // report data (will be loaded from SQL by report itself)
									   , qsTr("Competitors with card lent") // report preview window title
									   , "" // persistent settings ID
									   , {eventPlugin: FrameWork.plugin("Event")});
	}

}
