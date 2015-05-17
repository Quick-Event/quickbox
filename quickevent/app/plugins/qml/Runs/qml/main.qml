import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0

RunsPlugin {
	id: root

	property QfObject internals: QfObject {
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
		SqlTableModel {
			id: reportModel
		}
	}

	property list<Action> actions: [
		Action {
			id: act_print_startList_classes
			text: qsTr('&Classes')
			onTriggered: {
				root.printStartListClasses()
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a = a.addMenuInto("startList", "&Start list");
		a.addActionInto(act_print_startList_classes);
	}

	function printStartListClasses()
	{
		Log.info("runs printStartListClasses triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		//var stage_data = event_plugin.stageDataMap(stage_id);
		var tt = new TreeTable.Table();

		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name')
			.select2('courses', 'length, climb')
			.from('classes')
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stageId}}")
			.join("classdefs.courseId", "courses.id")
			.orderBy('classes.name');//.limit(1);
		reportModel.setQueryParameters({stageId: stage_id})
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stageId", stage_id)
		tt.setValue("event", event_plugin.eventConfig.value("event"));

		console.debug(tt.toString());

		reportModel.queryBuilder.clear()
			.select2('competitors', 'registration')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('runs', 'siId, startTimeMs')
			.from('competitors')
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
			.where("competitors.classId={{class_id}}")
			.orderBy('runs.startTimeMs');
		for(var i=0; i<tt.rowCount(); i++) {
			var class_id = tt.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.setQueryParameters({stage_id: stage_id, class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			tt.addTable(i, ttd);
		}
		console.debug(tt.toString());

		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Start list by clases");
		w.setReport(root.manifest.homeDir + "/reports/startList_classes.qml");
		w.setTableData(tt.data());
		var dlg = FrameWork.createQmlDialog();
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}
}
