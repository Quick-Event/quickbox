import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root
	//featureId: 'SqlDb'
	//dependsOnFeatureIds: "Core"

	property QtObject api: QtObject {
		property bool sqlServerConnected: false
		property string connectionType: ""
	}

	property QfObject internals: QfObject
	{
	SqlConnection {
		id: db
		/*
			Component.onCompleted: {
				//console.warn('COMPL');
				Sql.addDatabase('QFPSQL');
				// set connection name to reload new created driver
				db.connectionName = db.defaultConnectionName
			}
			*/
	}
	Component {
		id: dlgConnectDb
		DlgConnectDb {}
	}
	property list<Action> actions: [
		Action {
			id: actConnectDb
			text: qsTr('&Connect to database')
			//shortcut: "Ctrl+E"
			onTriggered: {
				Log.info(text, "triggered");
				connectToSqlServer();
			}
		}
	]
}

onInstalled:
{
	//_Plugin_install();
	var quit = FrameWork.menuBar.actionForPath('file/quit', false);
	quit.addActionBefore(actConnectDb);
	quit.addSeparatorBefore();

	FrameWork.pluginsLoaded.connect(postInstall);
}

function postInstall()
{
	connectToSqlServer();
}

function connectToSqlServer()
{
	console.debug("connectToSqlServer()");
	var cancelled = false;
	var connect_ok = false;
	var connection_type = "";

	var dlg = dlgConnectDb.createObject(FrameWork);
	while(!connect_ok) {
		cancelled = !dlg.loadSettingsAndExec();
		if(cancelled)
			break;
		var core_feature = FrameWork.plugin("Core");
		var settings = core_feature.api.createSettings();
		settings.beginGroup("sql");
		connection_type = settings.value("connectionType");
		console.debug("connection_type:", connection_type);
		if(connection_type) {
			var driver_name = "QSQLITE";
			if(connection_type == "sqlServer") {
				driver_name = "QPSQL";
			}
			connect_ok = Sql.addDatabase(driver_name);
			Log.info("Adding database driver:", driver_name, "OK:", connect_ok)
			if(connect_ok) {
				//db.setDefaultConnectionName();
				Log.info(db, db.connectionName, db.driverName);
				settings.beginGroup(connection_type);
				if(connection_type == "sqlServer") {
					db.hostName = settings.value('host');
					db.userName = settings.value('user');
					db.password = core_feature.api.crypt.decrypt(settings.value("password", ""));
					db.databaseName = 'quickevent';
					connect_ok = db.open();
				}
				else if(connection_type == "singleFile") {
					/*
					var wd = settings.value('workingDir');
					if(wd) {
						var event_name = settings.value('event');
						if(event_name) {
							var fn = wd + "/" + event_name + ".qbe";
							db.databaseName = fn;
							connect_ok = db.open();
						}
						else {
							// TODO: check if wd exists
							connect_ok = true;
						}
					}
					*/
				}
				settings.endGroup();
			}
		}
		settings.destroy();
		if(!connect_ok) {
			MessageBoxSingleton.critical(FrameWork, qsTr("Connect Database Error: %1").arg(db.errorString()));
		}
	}
	dlg.destroy();
	//Log.info("settinmg API connection type to:", connection_type);
	root.api.connectionType = connection_type;
	root.api.sqlServerConnected = connect_ok;
}

}
