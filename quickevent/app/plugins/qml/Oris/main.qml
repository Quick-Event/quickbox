import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internal: QfObject
	{
		//property Plugin pluginSqlDb: FrameWork.plugin("SqlDb")
		SqlConnection {
			id: db
		}
		Importer {
			id: orisImporter
		}
	}

	property list<Action> actions: [
		Action {
			id: actImportEventOris
			text: qsTr('From &Oris')
			//enabled: internal.pluginSqlDb.api.sqlServerConnected
			onTriggered: {
				orisImporter.chooseAndImport()
			}
		}
	]

	onInstalled:
	{
		var quit = FrameWork.menuBar.actionForPath('file/quit');
		quit.addMenuBefore('importEvent', qsTr('&Import event'));
		quit.addSeparatorBefore();
		FrameWork.menuBar.actionForPath('file/importEvent').addActionInto(actImportEventOris);
	}

}
