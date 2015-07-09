import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
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

	function nStagesClassesTable(stages_count)
	{
		var event_plugin = FrameWork.plugin("Event");

		var tt = new TreeTable.Table();
		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name')
			.from('classes')
			.orderBy('classes.name');//.limit(1);
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stagesCount", stages_count)
		tt.setValue("event", event_plugin.eventConfig.value("event"));

		for(var i=0; i<tt.rowCount(); i++) {
			var class_id = tt.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.queryBuilder.clear()
				.select2('competitors', 'registration, lastName, firstName, siId')
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.from('competitors')
				.where("competitors.classId={{class_id}}")
				.orderBy("competitors.lastName, competitors.registration");
			for(var stage_id = 1; stage_id <= stages_count; stage_id++) {
				var runs_table = "runs" + stage_id;
				reportModel.queryBuilder
					.select2(runs_table, "siid, startTimeMs")
					.joinRestricted("competitors.id", "runs.competitorId AS " + runs_table, runs_table + ".stageId=" + stage_id + " AND NOT " + runs_table + ".offRace")
			}
			reportModel.setQueryParameters({class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			var tt2 = new TreeTable.Table(ttd);
			tt.addTable(i, ttd);
		}
		//console.info(tt.toString());
		return tt;
	}

	function nStagesClubsTable(stages_count)
	{
		var event_plugin = FrameWork.plugin("Event");

		var tt = new TreeTable.Table();
		var qs1 = "SELECT * FROM ( SELECT substr(registration, 1, 3) AS clubAbbr FROM competitors) AS t GROUP BY clubAbbr ORDER BY clubAbbr";
		reportModel.query = "SELECT t2.clubAbbr, clubs.name FROM ( " + qs1 + " ) AS t2"
				+ " LEFT JOIN clubs ON t2.clubAbbr=clubs.abbr"
				+ " ORDER BY t2.clubAbbr";// + " LIMIT 1";
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stagesCount", stages_count)
		tt.setValue("event", event_plugin.eventConfig.value("event"));

		for(var i=0; i<tt.rowCount(); i++) {
			var club_abbr = tt.value(i, "clubAbbr");
			//console.debug("class id:", class_id);
			reportModel.queryBuilder.clear()
				.select2('competitors', 'registration, siId')
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.select2('classes', 'name')
				.from('competitors')
				.join("competitors.classId", "classes.id")
				.where("substr(competitors.registration, 1, 3)='{{club_abbr}}'")
				.orderBy('classes.name, competitors.lastName');
			for(var stage_id = 1; stage_id <= stages_count; stage_id++) {
				var runs_table = "runs" + stage_id;
				reportModel.queryBuilder
					.select2(runs_table, "siid, startTimeMs")
					.joinRestricted("competitors.id", "runs.competitorId AS " + runs_table, runs_table + ".stageId=" + stage_id + " AND NOT " + runs_table + ".offRace")
			}
			reportModel.setQueryParameters({club_abbr: club_abbr});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			var tt2 = new TreeTable.Table(ttd);
			tt.addTable(i, ttd);
		}
		console.debug(tt.toString());
		return tt;
	}

	function printClassesNStages()
	{
		Log.info("runs startLists printClassesNStages triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var n = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of stages:"), stage_id, 1, event_plugin.stageCount);
		var tt = nStagesClassesTable(n);
		//console.info("n:", n)
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/startLists_classes_nstages.qml"
									   , tt.data()
									   , qsTr("Start list by clases")
									   , ""
									   , {stageCount: n});
	}

	function printClubsNStages()
	{
		Log.info("runs startLists printClubsNStages triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var n = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of stages:"), stage_id, 1, event_plugin.stageCount);
		var tt = nStagesClubsTable(n);
		//console.info("n:", n)
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/startLists_clubs_nstages.qml"
									   , tt.data()
									   , qsTr("Start list by clubs")
									   , ""
									   , {stageCount: n});
	}

}
