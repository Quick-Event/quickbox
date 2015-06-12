import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

QtObject {
	id: root
	property RunsPlugin runsPlugin


	property QfObject internals: QfObject {
		SqlTableModel {
			id: reportModel
		}
	}

	function currentStageTable()
	{
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var tt = new TreeTable.Table();

		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name')
			.select2('courses', 'length, climb')
			.from('classes')
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.orderBy('classes.name');//.limit(1);
		reportModel.setQueryParameters({stage_id: stage_id})
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stageId", stage_id)
		tt.setValue("event", event_plugin.eventConfig.value("event"));

		reportModel.queryBuilder.clear()
			.select2('competitors', 'registration')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('runs', 'siId, timeMs, offRace, disqualified, cardError')
			.from('competitors')
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.status='FINISH'", "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy('runs.disqualified, runs.timeMs');
		for(var i=0; i<tt.rowCount(); i++) {
			var class_id = tt.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.setQueryParameters({stage_id: stage_id, class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			tt.addTable(i, ttd);
		}
		console.debug(tt.toString());
		return tt;
	}

	function printCurrentStage()
	{
		Log.info("runs printResultsCurrentStage triggered");
		var tt = currentStageTable();
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_stage.qml", tt.data(), qsTr("Start list by clases"));
	}

}
