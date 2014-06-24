import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgConnectDb"
	property var settings: Settings {}
	Frame {
		layoutType: Frame.LayoutForm
		LineEdit {
			Layout.buddyText: qsTr("&Host")
			text: "localhost"
		}
		LineEdit {
			Layout.buddyText: qsTr("&User")
			text: ""
		}
		LineEdit {
			Layout.buddyText: qsTr("&Password")
			echoMode: LineEdit.Password
			text: ""
		}
	}
	Component.onCompleted: {
		root.loadPersistentSettings(true);
	}
}
