import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property list<Action> actions: [
		Action {
			id: actReportLentCards
			text: qsTr('Pure QML report example - Competitors with rented cards')
			onTriggered: {
				printCompetitorsWithCardLent();
			}
		},
		Action {
			id: actTestReport
			text: qsTr('Test report')
			onTriggered: {
				Log.info("Example pactTestReport triggered");
				QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/testReport.qml" //report
											   , null // report data (will be loaded from SQL by report itself)
											   , qsTr("Test report") // report preview window title
											   , "" // persistent settings ID
											   , {eventPlugin: FrameWork.plugin("Event")});
			}
		}
	]

	onInstalled:
	{
		//console.warn("Oris installed");
		var act = FrameWork.menuBar.actionForPath('help/Examples/Reports');
		act.addActionInto(actReportLentCards)
		act.addActionInto(actTestReport)
	}

	function printCompetitorsWithCardLent()
	{
		Log.info("Example printCompetitorsWithCardLent triggered");
		QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/competitorsWithCardLent.qml" //report
									   , null // report data (will be loaded from SQL by report itself)
									   , qsTr("Competitors with rented cards") // report preview window title
									   , "" // persistent settings ID
									   , {eventPlugin: FrameWork.plugin("Event")});
	}

}
