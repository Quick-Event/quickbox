import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgConnectDb"
	/*
	segfault on Settings destruction, don't know why
	I guess that culprit is parenting non visual Settings to Widget.
	Same is with QtObject
	*/
	// property var settings: Settings { objectName: "kkt" }
	Frame {
		layoutType: Frame.LayoutForm
		LineEdit {
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
	}

	onAboutToBeDone: {
		if(result === Dialog.ResultAccept) {
            if(edUser.text.length == 0) {
                Log.info("user is empty");
                doneCancelled = true;
            }
		}
	}
}
