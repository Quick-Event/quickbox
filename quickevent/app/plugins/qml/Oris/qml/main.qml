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
			onTriggered: {
				orisImporter.syncCurrentEventEntries()
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
		}
	]

	onInstalled:
	{
		//console.warn("Oris installed");
		var act_import_oris = FrameWork.menuBar.actionForPath('file/import/oris');
		act_import_oris.text = qsTr("&Oris");
		act_import_oris.addActionInto(actImportEventOris);
		act_import_oris.addActionInto(actSyncCurrentEventEntries);
		act_import_oris.addSeparatorInto();
		act_import_oris.addActionInto(actImportClubsOris);
		act_import_oris.addActionInto(actImportRegistrationsOris);
		//act_import_oris.addActionInto(actTest)
		//quit.addMenuBefore('importEvent', qsTr('&Import event'));
		//quit.addSeparatorBefore();
		//FrameWork.menuBar.actionForPath('file/importEvent').addActionInto(actImportEventOris);
	}

}
