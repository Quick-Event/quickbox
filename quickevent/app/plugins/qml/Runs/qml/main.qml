import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0

RunsPlugin {
	id: root

	property QfObject internals: QfObject
	{
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
		ReportModel {
			id: reportModel
		}
	}

	property list<Action> actions: [
		Action {
			id: actPrintAll
			text: qsTr('All')
			onTriggered: {
				root.printAll()
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a.addActionInto(actPrintAll);		
	}

	function printAll()
	{
		Log.info("runs print all triggered");
		var stage_id = FrameWork.plugin("Event").currentStageId;
		reportModel.setQueryParameters({stageId: stage_id})
		reportModel.reload();
		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Runs");
		w.setReport(root.manifest.homeDir + "/reports/list.qml");
		w.setTableData(reportModel.toTreeTableData());
		var dlg = FrameWork.createQmlDialog();
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}
}
