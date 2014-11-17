import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgConnectDb"
	property Plugin coreFeature: FrameWork.plugin("Core")
	Frame {
		layoutType: Frame.LayoutForm
		LineEdit {
			id: edHost
			Layout.buddyText: qsTr("&Host")
			text: "localhost"
		}
		LineEdit {
			id: edUser
			Layout.buddyText: qsTr("&User")
		}
		LineEdit {
			id: edPassword
			Layout.buddyText: qsTr("&Password")
			echoMode: LineEdit.Password
		}
		LineEdit {
			id: edEvent
			Layout.buddyText: qsTr("&Event")
			placeholderText: qsTr("Event can be specified later on")
		}
	}

	buttonBox: ButtonBox {}

	Component.onCompleted: {
		//root.loadPersistentSettings(true);
		loadSettings();
		//gc();
	}

	function loadSettings()
	{
		var settings = coreFeature.api.createSettings();
		//Log.info("got settings:", settings);
		settings.beginGroup("sql/connection");
		edHost.text = settings.value("host", "localhost");
		edUser.text = settings.value("user", "");
		edPassword.text = coreFeature.api.crypt.decrypt(settings.value("password", ""));
		edEvent.text = settings.value("event", "");
		settings.destroy();
	}

	function saveSettings()
	{
		var settings = coreFeature.api.createSettings();
		settings.beginGroup("sql/connection");
		settings.setValue("host", edHost.text);
		settings.setValue("user", edUser.text);
		settings.setValue("password", coreFeature.api.crypt.encrypt(edPassword.text));
		settings.setValue("event", edEvent.text);
		settings.destroy();
	}

	function doneRequest_qml(result)
	{
		var ret = true;
		if(result === Dialog.ResultAccept) {
            if(!edUser.text) {
                MessageBoxSingleton.critical(root, qsTr("user is empty"));
                Log.info("user is empty");
                ret = false;
            }
            if(ret) {
            	saveSettings();
            }
		}
		return ret;
	}
}
