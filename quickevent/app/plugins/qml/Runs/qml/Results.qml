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

	function exportIofXml()
	{
		var default_file_name = "results-iof.xml";

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
		ev.push(['Organiser' ['Club', ['ShortName', "NIY"]]]);
		ev.push(['EventOfficial', ['Person', ['PersonName', ['Family', event.director]]]]);
		ev.push(['EventOfficial', ['Person', ['PersonName', ['Family', event.mainReferee]]]]);

		for(var i=0; i<tt1.rowCount(); i++) {
			var class_result = ['ClassResult'];
			result_list.push(class_result);
			class_result.push(['ClassShortName', tt1.value(i, "classes.name")]);
			var tt2 = tt1.table(i);
			for(var j=0; j<tt2.rowCount(); j++) {
				pos += 1
				var person_result = ['PersonResult'];
				class_result.push(person_result);
				var person = ['Person'];
				person_result.push(person);
				family = tt2.value(j, "competitors.lastName");
				given = tt2.value(j, "competitors.firstName");
				person.push(['PersonName', ['Family', family], ['Given', given]]);
				person.push(['PersonId', tt2.value(j, "registration")]);
				var result = ['Result'];
				person_result.push(result);
				result.push(['StartTime', ['Clock' + secToTimeStr(AMPMC(runner.stime), separator=':') + '</Clock></StartTime>')
				if runner.ftime >= 0:
					printout('<FinishTime><Clock>' + secToTimeStr(AMPMC(runner.ftime), separator=':') + '</Clock></FinishTime>')
				printout('<Time>', secToOBTime(runner.laptime, separator=':'), '</Time>')
				competitor_status = 'OK'
				if runner.status == 'DISK': competitor_status = 'MisPunch'
				elif runner.status == 'NOT_RUN': competitor_status = 'DidNotFinish'
				if competitor_status == 'OK': printout('<ResultPosition>', pos, '</ResultPosition>')
				printout('<CompetitorStatus value="' + competitor_status + '"/>')
				printout('<CourseLength unit="m">', katsplit.get('classinfo').get('length'), '</CourseLength>')
				printout('<CourseClimb unit="m">', katsplit.get('classinfo').get('climb'), '</CourseClimb>') # porusuje IOF DTD
				no = 0
				for punch in runner.punches:
					no += 1
					printout('<SplitTime sequence="%s">' % no)
					printout('<ControlCode>', punch.code, '</ControlCode>')
					printout('<Time>', secToOBTime(punch.stp, separator=':'), '</Time>')
					printout('</SplitTime>')
				printout('</Result>')
				printout('</PersonResult>')
			}
		}

//				printout('''<?xml version="1.0" encoding="{0}"?>
//			<!DOCTYPE ResultList SYSTEM "IOFdata.dtd">
//			<ResultList status="complete">
//				<IOFVersion version="2.0.3" />
//			'''.format(o_defaultEncoding))

				var file_name = File.tempPath() + "/quickevent/e" + tt1.value("stageId");
				if(File.mkpath(file_name)) {
					file_name += "/" + default_file_name;
					File.writeHtml(file_name, body, {documentTitle: qsTr("Start list by classes")});
					Log.info("exported:", file_name);
					return file_name;
				}
				return "";
	}

}
