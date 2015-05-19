import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

RunsPlugin {
	id: root

	property QfObject internals: QfObject {
		/*
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
		*/
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
		},
		Action {
			id: act_print_startList_clubs
			text: qsTr('C&lubs')
			onTriggered: {
				root.printStartListClubs()
			}
		},
		Action {
			id: act_export_html_startList_classes
			text: qsTr('&Classes')
			onTriggered: {
				root.exportHtmlStartListClasses()
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", true);
		a.text = qsTr("&Print");
		a = a.addMenuInto("startList", "&Start list");
		a.addActionInto(act_print_startList_classes);
		a.addActionInto(act_print_startList_clubs);

		a = root.partWidget.menuBar.actionForPath("exportHtml", true);
		a.text = qsTr("E&xport");
		a = a.addMenuInto("html", "&HTML");
		a = a.addMenuInto("startList", "&Start list");
		a.addActionInto(act_export_html_startList_classes);
	}

	function startListClassesTable()
	{
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

		//console.debug(tt.toString());

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
		return tt;
	}

	function startListClubsTable()
	{
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;
		var tt = new TreeTable.Table();

		var qs1 = "SELECT * FROM ( SELECT substr(registration, 1, 3) AS clubAbbr FROM competitors) AS t GROUP BY clubAbbr ORDER BY clubAbbr";
		reportModel.query = "SELECT clubs.* FROM ( " + qs1 + " ) AS t2"
				+ " LEFT JOIN clubs ON t2.clubAbbr=clubs.abbr"
				+ " WHERE clubs.id IS NOT NULL"
				+ " ORDER BY clubs.abbr";
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("stageId", stage_id)
		tt.setValue("event", event_plugin.eventConfig.value("event"));
		console.debug(tt.toString());

		reportModel.queryBuilder.clear()
			.select2('competitors', 'registration')
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2('classes', 'name')
			.select2('runs', 'siId, startTimeMs')
			.from('competitors')
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
			.join("competitors.classId", "classes.id")
			.where("substr(competitors.registration, 1, 3)='{{club_abbr}}'")
			.orderBy('classes.name, runs.startTimeMs');
		for(var i=0; i<tt.rowCount(); i++) {
			var club_abbr = tt.value(i, "abbr");
			console.debug("club_abbr:", club_abbr);
			reportModel.setQueryParameters({club_abbr: club_abbr, stage_id: stage_id});
			reportModel.reload();
			var ttd = reportModel.toTreeTableData();
			tt.addTable(i, ttd);
		}
		return tt;
	}

	function printStartListClasses()
	{
		Log.info("runs printStartListClasses triggered");
		var tt = startListClassesTable();
		QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/startList_classes.qml", tt.data(), qsTr("Start list by clases"));
		/*
		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Start list by clases");
		w.setReport(root.manifest.homeDir + "/reports/startList_classes.qml");
		w.setTableData(tt.data());
		var dlg = FrameWork.createQmlDialog();
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
		*/
	}

	function printStartListClubs()
	{
		Log.info("runs printStartListClubs triggered");
		var tt = startListClubsTable();
		QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/startList_clubs.qml", tt.data(), qsTr("Start list by clubs"));
	}

	function exportHtmlStartListClasses()
	{
		var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), "startlist-classes.html", qsTr("HTML files (*.html)"));
		//Log.info(file_name);
		if(!file_name)
			return;
		var tt1 = startListClassesTable();
		var body = ['body']
		var h1_str = "{{documentTitle}}";
		var event = tt1.value("event");
		if(event.stageCount > 1)
			h1_str = "E" + tt1.value("stageId") + " " + h1_str;
		body.push(['h1', h1_str]);
		body.push(['h2', event.name]);
		body.push(['h3', event.place]);
		body.push(['h3', event.date]);
		var div1 = ['div'];
		body.push(div1);
		for(var i=0; i<tt1.rowCount(); i++) {
			div1.push(['a', {"href": "#class_" + tt1.value(i, 'classes.name')}, tt1.value(i, 'classes.name')], "nbsp;")
		}
		for(var i=0; i<tt1.rowCount(); i++) {
			div1 = ['h2', ['a', {"name": "class_" + tt1.value(i, 'classes.name')}, tt1.value(i, 'classes.name')]];
			body.push(div1);
			div1 = ['h3', qsTr("length:"), tt1.value(i, 'courses.length'), qsTr("climb:"), tt1.value(i, 'courses.climb')];
			body.push(div1);
			var table = ['table'];
			body.push(table);
			var tt2 = tt1.table(i);
			var tr = ['tr',
					  ['th', qsTr("Start")],
					  ['th', qsTr("Name")],
					  ['th', qsTr("Registration")],
					  ['th', qsTr("SI")]
					];
			table.push(tr);
			for(var j=0; j<tt2.rowCount(); j++) {
				tr = ['tr'];
				if(j % 2)
					tr.push({"class": "odd"});
				tr.push(['td', OGTime.msecToString(tt2.value(j, 'startTimeMs'))]);
				tr.push(['td', tt2.value(j, 'competitorName')]);
				tr.push(['td', tt2.value(j, 'registration')]);
				tr.push(['td', tt2.value(j, 'runs.siId')]);
				table.push(tr);
			}
		}
		//var s = JSON.stringify(html, null, 2);
		File.writeHtml(file_name, body, {documentTitle: qsTr("Start list by classes")});
	}
}
