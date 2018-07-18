import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Runs 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
import "qrc:/quickevent/core/js/ogtime.js" as OGTime
import "./"


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
			id: act_export_startList_iofxml3
			text: qsTr('&IOF-XML 3.0')
			onTriggered: {
				var default_file_name = "startlist-iof3.xml";
				var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("XML files (*.xml)"));
				if(file_name)
					startLists.exportStartListIofXml3(file_name)
			}
		},
		Action {
			id: act_export_startList_emma
			text: qsTr('&EmmaClient')
			onTriggered: {
				var default_file_name = "emma-start.txt";
				var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("TXT files (*.txt)"));
				if(file_name)
					startLists.exportStartListEmma(file_name)
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
			id: act_print_competitorswithCardRent
			text: qsTr('&Competitors with card rent')
			onTriggered: {
				Log.info("act_print_competitorswithCardRent triggered");
				QmlWidgetsSingleton.showReport(root.manifest.homeDir + "/reports/competitorsWithCardRent.qml" //report
											   , null // report data (will be loaded from SQL by report itself)
											   , qsTr("Competitors with card rent") // report preview window title
											   , "" // persistent settings ID
											   , {stageId: root.selectedStageId});
			}
		},
		Action {
			id: act_export_results_iofxml
			text: qsTr('&IOF XML 2.3')
			onTriggered: {
				var default_file_name = "results-iof.xml";
				var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("XML files (*.xml)"));
				results.exportIofXml2(file_name)
			}
		},
		Action {
			id: act_export_results_winsplits
			text: qsTr('&WinSplits')
			onTriggered: {
				var default_file_name = "winsplits-iofxml-23.xml";
				var file_name = InputDialogSingleton.getSaveFileName(null, qsTr("Get file name"), default_file_name, qsTr("XML files (*.xml)"));
				results.exportIofXml2(file_name, true)
			}
		}
	]

	onNativeInstalled:
	{
		var a_print = root.partWidget.menuBar.actionForPath("print", false);

		var a = a_print.addMenuInto("startList", "&Start list");
		a.addActionInto(act_print_startList_classes);
		a.addActionInto(act_print_startList_clubs);
		a.addActionInto(act_print_startList_starters);
		a.addSeparatorInto();
		a.addActionInto(act_print_startList_classes_nstages);
		a.addActionInto(act_print_startList_clubs_nstages);

		a = a_print.addMenuInto("results", "&Results");
		a.addActionInto(act_print_results_currentStage);
		a.addActionInto(act_print_results_currentStageFirstN);
		a.addSeparatorInto("results_awards_separator");
		a.addActionInto(act_print_results_currentStageAwards);
		a.addSeparatorInto("results_nstages_separator");
		a.addActionInto(act_print_results_nStages);
		a.addActionInto(act_print_results_NStageAwards);

		var a_sep = a_print.addSeparatorInto();
		a_sep.addActionAfter(act_print_competitorswithCardRent)

		var a_export = root.partWidget.menuBar.actionForPath("export", false);

		var m_stlist = a_export.addMenuInto("startList", "&Start list");
		a = m_stlist.addMenuInto("html", "&HTML");
		a.addActionInto(act_export_html_startList_classes);
		a.addActionInto(act_export_html_startList_clubs);
		a = m_stlist.addMenuInto("xml", "&XML");
		a.addActionInto(act_export_startList_iofxml3);
		a = m_stlist.addMenuInto("txt", "&TXT");
		a.addActionInto(act_export_startList_emma);

		//var m_results = a_export.addMenuInto("results", "&Results");
		var m_results = root.partWidget.menuBar.actionForPath("export/results", false);
		m_results.addActionInto(act_export_results_iofxml);
		m_results.addActionInto(act_export_results_winsplits);

		//Log.warning("onNativeInstalled");

	}
}
