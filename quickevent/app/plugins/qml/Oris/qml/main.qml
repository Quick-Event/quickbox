import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Oris 1.0

Oris {
	id: root

	property QfObject internal: QfObject
	{
		OrisImporter {
			id: orisImporter
		}
		TxtImporter {
			id: txtImporter
		}
	}

	property list<Action> actions: [
		Action {
			id: actImportEventOris
			text: qsTr('&Event')
			//enabled: internal.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				orisImporter.chooseAndImport()
			}
		},
		Action {
			id: actSyncCurrentEventEntries
			text: qsTr('&Sync current event entries')
			enabled: false
			onTriggered: {
				orisImporter.syncCurrentEventEntries()
			}
		},
		Action {
			id: actSyncRelaysEntriesOris
			text: qsTr('&Sync relays entries')
			enabled: false
			onTriggered: {
				orisImporter.syncRelaysEntries()
			}
		},
		Action {
			id: actImportClubsOris
			text: qsTr('&Clubs')
			//enabled: internal.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				orisImporter.importClubs();
			}
		},
		Action {
			id: actImportRegistrationsOris
			text: qsTr('&Registrations')
			//enabled: internal.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				orisImporter.importRegistrations();
			}
		},
		Action {
			id: actImportCompetitorsCSOS
			text: qsTr('&Competitors CSOS')
			onTriggered: {
				txtImporter.importCompetitorsCSOS();
			}
		},
		Action {
			id: actImportCompetitorsCSV
			text: qsTr('&Competitors CSV')
			onTriggered: {
				txtImporter.importCompetitorsCSV();
			}
		},
		Action {
			id: actImportRankingCsv
			text: qsTr('&Ranking CSV (Oris format)')
			onTriggered: {
				txtImporter.importRankingCsv();
			}
		}
	]

	onInstalled:
	{
		//console.warn("Oris installed");
		var act_import_oris = FrameWork.menuBar.actionForPath('file/import/oris');
		act_import_oris.text = qsTr("&Oris");
		act_import_oris.enabled = false;
		act_import_oris.addActionInto(actImportEventOris);
		act_import_oris.addActionInto(actSyncCurrentEventEntries);
		//act_import_oris.addSeparatorInto();
		//act_import_oris.addActionInto(actSyncRelaysEntriesOris);
		act_import_oris.addSeparatorInto();
		act_import_oris.addActionInto(actImportClubsOris);
		act_import_oris.addActionInto(actImportRegistrationsOris);

		var act_import_txt = FrameWork.menuBar.actionForPath('file/import/txt');
		act_import_txt.text = qsTr("&Text file");
		act_import_txt.enabled = false;
		act_import_txt.addActionInto(actImportCompetitorsCSOS);
		act_import_txt.addActionInto(actImportCompetitorsCSV);
		act_import_txt.addActionInto(actImportRankingCsv);

		var refreshActions1 = function(is_db_open)
		{
			act_import_oris.enabled = is_db_open;
			act_import_txt.enabled = is_db_open;
		}
		FrameWork.plugin("Event").sqlServerConnectedChanged.connect(refreshActions1);

		var refreshActions2 = function(event_name)
		{
			console.warn("refresh actions event open:", event_name);
			actSyncCurrentEventEntries.enabled = event_name;
		}
		FrameWork.plugin("Event").eventOpenChanged.connect(refreshActions2);
	}

}
