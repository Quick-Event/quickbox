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
		CompetitorsListModel {
			id: competitorsModel
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
		Log.info("competitors print all triggered");
		competitorsModel.reload();
		var tt = new TreeTable.Table();
		tt.setData(competitorsModel.toTreeTableData());
		tt.setValue("title", "Competitors list")
		//console.warn("tt1", tt.toString());
		tt.addColumn("test_col");
		for(var i=0; i<tt.rowCount(); i++)
			tt.setValue(i, "test_col", "test_data_" + i);
		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Competitors");
		w.setReport(root.manifest.homeDir + "/reports/list.qml");
		w.setData(tt.data());
		var dlg = FrameWork.createQmlDialog();
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}
}
