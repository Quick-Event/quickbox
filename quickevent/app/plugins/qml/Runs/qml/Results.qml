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

	function currentStageTableData(class_filter, max_competitors_in_class)
	{
		var td = runsPlugin.currentStageResultsTableData(class_filter, max_competitors_in_class);
		return td;
	}

	function currentStageTable(class_filter, max_competitors_in_class)
	{
		var td = runsPlugin.currentStageResultsTableData(class_filter, max_competitors_in_class);
		var tt = new TreeTable.Table();
		tt.setData(td);
		return tt;
	}

	/*
	function currentStageTable(class_filter, max_competitors_in_class)
	{
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = runsPlugin.selectedStageId;
		var tt = new TreeTable.Table();

		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name')
			.select2('courses', 'length, climb')
			.from('classes')
			//.where("classes.name NOT IN ('D21B', 'H40B', 'H35C', 'H55B')")
			.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
			.join("classdefs.courseId", "courses.id")
			.orderBy('classes.name');//.limit(1);
		if(class_filter) {
			reportModel.queryBuilder.where(class_filter);
		}

		reportModel.setQueryParameters({stage_id: stage_id})
		console.info("currentStageTable query:", reportModel.effectiveQuery());
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stageId", stage_id)
		tt.setValue("event", event_plugin.eventConfig.value("event"));

		reportModel.queryBuilder.clear()
			.select2('competitors', 'registration, lastName, firstName')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('runs', '*')
			.select2('clubs', 'name')
			.from('competitors')
			.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
			//.join("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy("runs.notCompeting, runs.disqualified, runs.timeMs");
		if(max_competitors_in_class)
			reportModel.queryBuilder.limit(max_competitors_in_class);
		for(var i=0; i<tt.rowCount(); i++) {
			var class_id = tt.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.setQueryParameters({stage_id: stage_id, class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			var tt2 = new TreeTable.Table(ttd);
			tt2.addColumn("pos", "int");
			for(var j=0; j<tt2.rowCount(); j++) {
				var has_pos = !tt2.value(j, "disqualified") && !tt2.value(j, "notCompeting");
				if(has_pos)
					tt2.setValue(j, "pos", j+1);
				else
					tt2.setValue(j, "pos", '');
			}
			tt.addTable(i, ttd);
		}
		console.debug(tt.toString());
		return tt;
	}
	*/
	/*
	function currentStageAwardsTable(max_competitors_in_class)
	{
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = runsPlugin.selectedStageId;
		var tt = new TreeTable.Table();

		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name')
			.from('classes')
			//.where("name NOT IN ('D21B', 'H40B', 'H35B', 'H55B')")
			.orderBy('classes.name');//.limit(1);
		//reportModel.setQueryParameters({stage_id: stage_id})
		reportModel.reload();
		var tt_classes = new TreeTable.Table(reportModel.toTreeTableData());

		reportModel.queryBuilder.clear()
			.select2('competitors', 'registration, lastName, firstName')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('runs', '*')
			.select2('clubs', 'name')
			.from('competitors')
			.join("LEFT JOIN clubs ON substr(competitors.registration, 1, 3) = clubs.abbr")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}"
							+ " AND runs.isRunning"
							+ " AND NOT runs.disqualified"
							+ " AND NOT runs.notCompeting"
							+ " AND runs.finishTimeMs>0", "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy('runs.timeMs');
		if(max_competitors_in_class)
			reportModel.queryBuilder.limit(max_competitors_in_class);
		for(var i=0; i<tt_classes.rowCount(); i++) {
			var class_id = tt_classes.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.setQueryParameters({stage_id: stage_id, class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			var tt2 = new TreeTable.Table(ttd);
			tt2.addColumn("pos", "int");
			tt2.addColumn("className", "QString");
			for(var j=0; j<tt2.rowCount(); j++) {
				tt2.setValue(j, "pos", j+1);
				tt2.setValue(j, "className", tt_classes.value(i, "classes.name"));
			}
			tt2.setValue("eventConfig", event_plugin.eventConfig.values());
			tt2.setValue("director", event_plugin.eventConfig.director());
			tt2.setValue("mainReferee", event_plugin.eventConfig.mainReferee());
			if(tt.isNull()) {
				tt.setData(ttd);
			}
			else {
				var n = tt.rowCount();
				for(var k=0; k<tt2.rowCount(); k++) {
					for(var l=0; l<tt2.columnCount(); l++) {
						tt.setValue(n+k, l, tt2.value(k, l));
					}
				}
			}
		}
		//console.warn(tt.toString());
		return tt;
	}
*/
	function printCurrentStage()
	{
		Log.info("runs printResultsCurrentStage triggered");
		var dlg = runsPlugin.createReportOptionsDialog(FrameWork);
		dlg.persistentSettingsId = "resultsReportOptions";
		//dlg.dialogType = RunsPlugin.ResultsReport;
		//var mask = InputDialogSingleton.getText(this, qsTr("Get text"), qsTr("Class mask (use wild cards [*?]):"), "*");
		if(dlg.exec()) {
			var td = currentStageTableData(dlg.sqlWhereExpression());
			QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_stage.qml"
										   , td
										   , qsTr("Results by clases")
										   , "printCurrentStage"
										   , {isBreakAfterEachClass: dlg.isBreakAfterEachClass(), isColumnBreak: dlg.isColumnBreak()}
										   );
		}
		dlg.destroy();
	}

	function printCurrentStageFirstN()
	{
		Log.info("runs printCurrentStageFirstN triggered");
		var n = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Limit number of printed runners in each class to:"), 3, 1);
		var td = currentStageTableData("", n);
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_stageWide.qml", td, qsTr("Stage results by clases"));
	}

	function printCurrentStageAwards()
	{
		var opts = runsPlugin.printAwardsOptionsWithDialog();
		var rep_path = opts.reportPath;
		if(!rep_path)
			return;

		//var n = opts.numPlaces;
		//var tt = currentStageAwardsTable(n);
		var td = runsPlugin.currentStageResultsTableData("", opts.numPlaces);
		var tt = new TreeTable.Table();
		tt.setData(td);

		Log.info("runs printCurrentStageAwards", rep_path);
		QmlWidgetsSingleton.showReport(rep_path
									   , tt.data()
									   , qsTr("Stage awards")
									   , ""
									   , {eventConfig: FrameWork.plugin("Event").eventConfig});
	}

	function exportIofXml2(file_path)
	{
		var event_plugin = FrameWork.plugin("Event");
		var start00_msec = event_plugin.stageStartMsec(runsPlugin.selectedStageId);

		var tt1 = currentStageTable();
		var result_list = ['ResultList', {"status": "complete"}];
		result_list.push(['IOFVersion', {"version": "2.0.3"}]);
		var event = tt1.value("event");

		var ev = ['Event'];
		result_list.push(ev);
		ev.push(['EventId', {type: "int", idManager: "IOF"}, event.importId]);
		ev.push(['Name', event.name]);
		ev.push(['EventClassificationId', {type: "int", idManager: "IOF"}, 0]);
		ev.push(['StartDate', ['Date', TimeExt.dateToISOString(event.date)]]);
		ev.push(['Organiser', ['Club', ['ShortName', "NIY"]]]);
		ev.push(['EventOfficial', ['Person', ['PersonName', ['Family', event.director]]]]);
		ev.push(['EventOfficial', ['Person', ['PersonName', ['Family', event.mainReferee]]]]);

		for(var i=0; i<tt1.rowCount(); i++) {
			var class_result = ['ClassResult'];
			result_list.push(class_result);
			class_result.push(['ClassShortName', tt1.value(i, "classes.name")]);
			var tt2 = tt1.table(i);
			var pos = 0;
			for(var j=0; j<tt2.rowCount(); j++) {
				pos++;
				var person_result = ['PersonResult'];
				class_result.push(person_result);
				var person = ['Person'];
				person_result.push(person);
				var family = tt2.value(j, "competitors.lastName");
				var given = tt2.value(j, "competitors.firstName");
				person.push(['PersonName', ['Family', family], ['Given', given]]);
				person.push(['PersonId', tt2.value(j, "registration")]);
				var result = ['Result'];
				person_result.push(result);
				var stime = start00_msec + tt2.value(j, "startTimeMs");
				var ftime = start00_msec + tt2.value(j, "finishTimeMs");
				var time = tt2.value(j, "timeMs");
				if(ftime && time)
					stime = ftime - time; // cover cases when competitor didn't started according to start list from any reason
				result.push(['StartTime', ['Clock', TimeExt.msecToString_hhmmss(stime)]])
				if(ftime)
					result.push(['FinishTime', ['Clock', TimeExt.msecToString_hhmmss(ftime)]])
				result.push(['Time', {timeFormat: "MM:SS"}, OGTime.msecToString_mmss(time, ':')])
				var competitor_status = 'OK'
				if (!ftime) {
					 competitor_status = 'DidNotFinish'
				}
				else if (tt2.value(j, "disqualified")) {
					if (tt2.value(j, "misPunch"))
						competitor_status = 'MisPunch'
					else
						competitor_status = 'Disqualified'
				}
				else if (tt2.value(j, "notCompeting"))
					competitor_status = 'NotCompeting'
				if (competitor_status == 'OK')
					result.push(['ResultPosition', tt2.value(j, "pos")])
				result.push(['CompetitorStatus', {"value": competitor_status}])
				/*
				  according to DTD
				result.push(['CourseVariation'
							 , ['CourseLength', {unit: "m"}, tt1.value(i, "courses.length")]
							 , ['CourseClimb', {unit: "m"}, tt1.value(i, "courses.climb")]
							]);
				*/
				// working
				result.push(['CourseLength', {unit: "m"}, tt1.value(i, "courses.length")]);
				result.push(['CourseClimb', {unit: "m"}, tt1.value(i, "courses.climb")]);
				reportModel.queryBuilder.clear()
					.select2('runlaps', '*')
					.from('runlaps')
					.where("runlaps.runId={{run_id}}")
					.where("runlaps.code!=999") // omit finish lap
					.orderBy('runlaps.position');
				reportModel.setQueryParameters({run_id: tt2.value(j, "runs.id")})
				reportModel.reload();
				for(var k=0; k<reportModel.rowCount(); k++) {
					//console.info(k, reportModel.value(k, "position"));
					result.push(['SplitTime', {"sequence": reportModel.value(k, "position")}
								 , ['ControlCode', reportModel.value(k, "code")]
								 , ['Time', OGTime.msecToString_mmss(reportModel.value(k, "stpTimeMs"), ':')]
								]);
				}
			}
		}

		File.writeXml(file_path, result_list, {documentTitle: qsTr("E%1 IOF XML stage results").arg(tt1.value("stageId"))});
		Log.info("exported:", file_path);
	}

	function nStagesResultsTable(stages_count, places, exclude_disq)
	{
		var event_plugin = FrameWork.plugin("Event");

		var tt = new TreeTable.Table();
		tt.setData(runsPlugin.nstagesResultsTableData(stages_count, places, exclude_disq));
		tt.setValue("stagesCount", stages_count)
		tt.setValue("event", event_plugin.eventConfig.value("event"));
		tt.setValue("stageStart", event_plugin.stageStartDateTime(stages_count));
		//console.info(tt.toString());
		return tt;
	}

	function printNStages()
	{
		Log.info("runs results printNStages triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var dlg = runsPlugin.createNStagesReportOptionsDialog(FrameWork);
		dlg.stagesCount = stage_id;
		dlg.maxPlacesCount = 9999;
		dlg.excludeDisqualified = true;
		if(dlg.exec()) {
			var tt = nStagesResultsTable(dlg.stagesCount, dlg.maxPlacesCount, dlg.excludeDisqualified);
			QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_nstages.qml"
										   , tt.data()
										   , qsTr("Results after " + dlg.stagesCount + " stages")
										   , ""
										   , {stagesCount: dlg.stagesCount});
		}
		dlg.destroy();
		//var n = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of stages:"), stage_id, 1, event_plugin.stageCount);
		//var places = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of places in each class:"), 9999, 1);
	}

	function printNStageAwards()
	{
		Log.info("runs printNStageAwards triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var stage = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of stages:"), stage_id, 1, event_plugin.stageCount);
		var places = InputDialogSingleton.getInt(this, qsTr("Get number"), qsTr("Number of places in each class:"), 3, 1);
		var tt = nStagesResultsTable(stage, places);
		console.info(tt.toString());
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_nstages_awards.qml", tt.data(), qsTr("Stage awards"));
	}

}

