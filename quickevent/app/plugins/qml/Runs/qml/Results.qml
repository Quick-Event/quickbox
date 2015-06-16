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
			.select2('competitors', 'registration, lastName, firstName')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('runs', '*')
			.from('competitors')
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}} AND NOT runs.offRace AND runs.finishTimeMs>0", "JOIN")
			.where("competitors.classId={{class_id}}")
			.orderBy('runs.disqualified, runs.timeMs');
		for(var i=0; i<tt.rowCount(); i++) {
			var class_id = tt.value(i, "classes.id");
			console.debug("class id:", class_id);
			reportModel.setQueryParameters({stage_id: stage_id, class_id: class_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			var tt2 = new TreeTable.Table(ttd);
			tt2.addColumn("pos", "int");
			for(var j=0; j<tt2.rowCount(); j++) {
				var has_pos = !tt2.value(j, "disqualified")// && !tt2.value(j, "offResults");
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

	function printCurrentStage()
	{
		Log.info("runs printResultsCurrentStage triggered");
		var tt = currentStageTable();
		QmlWidgetsSingleton.showReport(runsPlugin.manifest.homeDir + "/reports/results_stage.qml", tt.data(), qsTr("Start list by clases"));
	}

	function exportIofXml(file_path)
	{
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
				var stime = tt2.value(j, "startTimeMs");
				var ftime = tt2.value(j, "finishTimeMs");
				var time = tt2.value(j, "timeMs");
				if(ftime && time)
					stime = ftime - time; // cover cases when competitor didn't started according to start list from any reason
				result.push(['StartTime', ['Clock', OGTime.msecToString(stime, ':')]])
				if(ftime)
					result.push(['FinishTime', ['Clock', OGTime.msecToString(ftime, ':')]])
				result.push(['Time', ['Clock', OGTime.msecToString(time, ':')]])
				var competitor_status = 'OK'
				if (!ftime)
					 competitor_status = 'DidNotFinish'
				if (tt2.value(j, "misPunch"))
					 competitor_status = 'MisPunch'
				else if (tt2.value(j, "disqualified"))
					 competitor_status = 'Disqualified'
				if (competitor_status == 'OK')
					result.push(['ResultPosition', tt2.value(j, "pos")])
				result.push(['CompetitorStatus', {"value": competitor_status}])
				result.push(['CourseVariation'
							 , ['CourseLength', {unit: "m"}, tt1.value(i, "courses.length")]
							 , ['CourseClimb', {unit: "m"}, tt1.value(i, "courses.climb")]
							]);
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
					result.push(['SplitTime', {"sequence": reportModel.value(k, "position")}]);
					result.push(['ControlCode', reportModel.value(k, "code")]);
					result.push(['Time', OGTime.msecToString(reportModel.value(k, "stpTimeMs"), ':')]);
				}
			}
		}

		File.writeXml(file_path, result_list, {documentTitle: qsTr("E%1 IOF XML stage results").arg(tt1.value("stageId"))});
		Log.info("exported:", file_path);
	}

}
