import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgConnectDb"
	//property Settings settings: Settings {}
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
			text: ""
		}
		LineEdit {
			Layout.buddyText: qsTr("&Password")
			echoMode: LineEdit.Password
			text: ""
		}
	}

	buttonBox: ButtonBox {}

	Component.onCompleted: {
		root.loadPersistentSettings(true);
		loadSettings();
	}

	function loadSettings()
	{
		var settings = FrameWork.plugin("Core").settings();
		//Log.info("got settings:", settings);
		settings.beginGroup("sql/connection");
		edHost.text = settings.value("host", "localhost")
		edUser.text = settings.value("user", "")
		settings.destroy();
	}

	function saveSettings()
	{
		var settings = FrameWork.plugin("Core").settings();
		settings.beginGroup("sql/connection");
		settings.setValue("host", edHost.text);
		settings.setValue("user", edUser.text);
		settings.destroy();
	}

	onAboutToBeDone: {
		/*
		I'm getting segfault on Settings destruction, if I don't set it to null before Dialog destruction.
		Don't know why, I guess that culprit is parenting non visual Settings to Widget.
		Same is with QtObject

		I've found different way in the end FrameWork.plugin("Core").settings();
		Leaving this here to not forget a nasty Qt/mine bug
		*/
		//settings = null;
		if(result === Dialog.ResultAccept) {
            if(edUser.text.length == 0) {
                Log.info("user is empty");
                doneCancelled = true;
            }
            if(!doneCancelled) {
            	saveSettings();
            }
		}
	}
}
