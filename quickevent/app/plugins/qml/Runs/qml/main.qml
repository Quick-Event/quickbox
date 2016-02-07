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
		StartLists {
			id: startLists
			runsPlugin: root
		}
		Results {
			id: results
			runsPlugin: root
		}
		//SqlTableModel {
		//	id: reportModel
		//}
	}

	property list<Action> actions: [
		Action {
			id: act_print_startList_classes
			text: qsTr('&Classes')
			onTriggered: {
				startLists.printStartListClasses()
			}
		},
		Action {
			id: act_print_startList_classes_nstages
			text: qsTr('Classes n stages')
			onTriggered: {
				startLists.printClassesNStages()
			}
		},
		Action {
			id: act_print_startList_clubs
			text: qsTr('C&lubs')
			onTriggered: {
				startLists.printStartListClubs()
			}
		},
		Action {
			id: act_print_startList_clubs_nstages
			text: qsTr('Clubs n stages')
			onTriggered: {
				startLists.printClubsNStages()
			}
		},
		Action {
			id: act_print_startList_starters
			text: qsTr('&Starters')
			onTriggered: {
				startLists.printStartListStarters()
			}
		},
		Action {
			id: act_export_html_startList_classes
			text: qsTr('&Classes')
			onTriggered: {
				var fn = startLists.exportHtmlStartListClasses()
				File.openUrl(File.toUrl(fn));
			}
		},
		Action {
			id: act_export_html_startList_clubs
			text: qsTr('&Clubs')
			onTriggered: {
				var fn = startLists.exportHtmlStartListClubs()
				File.openUrl(File.toUrl(fn));
			}
		},
		Action {
			id: act_print_results_currentStage
			text: qsTr('&Current stage')
			shortcut: "Ctrl+P"
			onTriggered: {
				results.printCurrentStage()
			}
		},
		Action {
			id: act_print_results_nStages
			text: qsTr('&After n stages')
			onTriggered: {
				results.printNStages()
			}
		},
		Action {
			id: act_print_results_currentStageFirstN
			text: qsTr('First &n competitors')
			onTriggered: {
				results.printCurrentStageFirstN()
			}
		},
		Action {
			id: act_print_results_NStageAwards
			text: qsTr('Awards after n stages')
			onTriggered: {
				results.printNStageAwards()
			}
		},
		Action {
			id: act_print_results_currentStageAwards
			text: qsTr('&Awards')
			onTriggered: {
				results.printCurrentStageAwards()
			}
		},
		Action {
			id: act_export_results_iofxml
			text: qsTr('&IOF XML 2.3')
			onTriggered: {
				var default_file_name = "results-iof.xml";
				var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("XML files (*.xml)"));
				results.exportIofXml(file_name)
			}
		}
	]

	onNativeInstalled:
	{
		var a = root.partWidget.menuBar.actionForPath("print", true);
		//a.text = qsTr("&Print");
		var a_print = a;
		a = a_print.addMenuInto("startList", "&Start list");
		a.addActionInto(act_print_startList_classes);
		a.addActionInto(act_print_startList_clubs);
		a.addActionInto(act_print_startList_starters);
		a.addSeparatorInto();
		a.addActionInto(act_print_startList_classes_nstages);
		a.addActionInto(act_print_startList_clubs_nstages);

		a = a_print.addMenuInto("results", "&Results");
		a.addActionInto(act_print_results_currentStage);
		a.addActionInto(act_print_results_currentStageFirstN);
		a.addActionInto(act_print_results_currentStageAwards);
		a.addSeparatorInto();
		a.addActionInto(act_print_results_nStages);
		a.addActionInto(act_print_results_NStageAwards);

		//var a_import = root.partWidget.menuBar.actionForPath("import", true);
		//a_import.text = qsTr("&Import");

		var a_export = root.partWidget.menuBar.actionForPath("export", true);
		//a_export.text = qsTr("E&xport");
		a = a_export.addMenuInto("startList", "&Start list");
		a = a.addMenuInto("html", "&HTML");
		a.addActionInto(act_export_html_startList_classes);
		a.addActionInto(act_export_html_startList_clubs);

		a = a_export.addMenuInto("results", "&Results");
		a.addActionInto(act_export_results_iofxml);

	}
}
