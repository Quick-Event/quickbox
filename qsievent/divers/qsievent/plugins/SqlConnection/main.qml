import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root
	property var dbSchema: DbSchema {}
	featureId: 'SqlConnection'
	//dependsOnFeatureIds: "Core"

	actions: [
		Action {
			id: actConnectDb
			text: qsTr('&Connect to database')
			//shortcut: "Ctrl+E"
			onTriggered: {
				Log.info(text, "triggered");
				connectToSqlServer(false);
			}
		},
		Action {
			id: actCreateEvent
			text: qsTr('Create new event')
			onTriggered: {
				Log.info(text, "triggered");
			}
		},
		Action {
			id: actOpenEvent
			text: qsTr('&Open event')
			shortcut: "Ctrl+O"
			onTriggered: {
				Log.info(text, "triggered");
			}
		}
	]

	property QfObject internals: QfObject
	{
		Component {
			id: dlgConnectDb
			DlgConnectDb {}
		}
	}

	function install()
	{
		//_Plugin_install();
		var quit = FrameWork.menuBar.actionForPath('file/quit', false);
		quit.prependAction(actConnectDb);
		quit.prependSeparator();
		quit.prependAction(actCreateEvent);
		quit.prependSeparator();

		FrameWork.postInstall.connect(postInstall);
	}

	function postInstall()
	{
		Sql.addDatabase('QPSQL');
		connectToSqlServer(false);
	}

	function connectToSqlServer(silent)
	{
		var cancelled = false;
		if(!silent) {
			var dlg = dlgConnectDb.createObject(FrameWork);
			cancelled = !dlg.exec();
			dlg.destroy();
		}
		if(!cancelled) {
			var core_feature = FrameWork.plugin("Core");
			var db = Sql.createDatabase();
			var settings = core_feature.createSettings();
			settings.beginGroup("sql/connection");
			db.hostName = settings.value('host');
			db.userName = settings.value('user');
			db.password = core_feature.crypt.decrypt(settings.value("password", ""));
			db.databaseName = 'quickevent';
			db.open();
			db.destroy();
			settings.destroy();
		}
	}
}
