import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root
	//featureId: 'SqlDb'
	//dependsOnFeatureIds: "Core"

	property QtObject api: QtObject
	{
		property bool sqlServerConnected: false
	}

	property list<Action> actions: [
		Action {
			id: actConnectDb
			text: qsTr('&Connect to database')
			//shortcut: "Ctrl+E"
			onTriggered: {
				Log.info(text, "triggered");
				connectToSqlServer(false);
			}
		}
	]

	property QfObject internals: QfObject
	{
		SqlConnection {
			id: db
			Component.onCompleted: {
				//console.warn('COMPL');
				Sql.addDatabase('QFPSQL');
				// set connection name to reload new created driver
				db.connectionName = db.defaultConnectionName
			}
		}
		Component {
			id: dlgConnectDb
			DlgConnectDb {}
		}
	}

	Component.onCompleted:
	{
		//_Plugin_install();
		var quit = FrameWork.menuBar.actionForPath('file/quit', false);
		quit.prependAction(actConnectDb);
		quit.prependSeparator();

		FrameWork.pluginsLoaded.connect(postInstall);
	}

	function postInstall()
	{
		connectToSqlServer(false);
	}

	function connectToSqlServer(silent)
	{
        var cancelled = false;
        var connect_ok = false;
		if(!silent) {
            var dlg = dlgConnectDb.createObject(FrameWork);
            while(!connect_ok) {
                cancelled = !dlg.exec();
                if(cancelled)
                    break;
                var core_feature = FrameWork.plugin("Core");
                //var db = Sql.database();
                var settings = core_feature.api.createSettings();
                settings.beginGroup("sql/connection");
                console.debug(db, db.driverName);
                db.hostName = settings.value('host');
                db.userName = settings.value('user');
                db.password = core_feature.api.crypt.decrypt(settings.value("password", ""));
                db.databaseName = 'quickevent';
                connect_ok = db.open();
                settings.destroy();
                if(!connect_ok) {
                    MessageBoxSingleton.critical(FrameWork, qsTr("Connect Database Error: %1").arg(db.errorString()));
                }
            }
            dlg.destroy();
		}
        root.api.sqlServerConnected = connect_ok;
	}
}
