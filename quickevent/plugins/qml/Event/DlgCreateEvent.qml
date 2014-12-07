import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Dialog {
	id: root

	windowTitle: qsTr("Create event")
	persistentSettingsId: "DlgCreateEvent"

	property string eventName: edEventName.text.replace(" ", "_").toLowerCase();
	property alias stageCount: edStageCount.value;

	Frame {
		layoutType: Frame.LayoutForm
		LineEdit {
			id: edEventName
			Layout.buddyText: qsTr("&Event name")
			text: "new_event"
		}
		SpinBox {
			id: edStageCount
			Layout.buddyText: qsTr("&Stage count")
			minimum: 1
			value: 1
		}
	}

	buttonBox: ButtonBox {}

	Component.onCompleted: {
		//root.loadPersistentSettings(true);
		//loadSettings();
		//console.warn("dialog completed")
		//gc();
	}

	function doneRequest_qml(result)
	{
		var ret = true;
		if(result === Dialog.ResultAccept) {
			if(!root.eventName) {
				MessageBoxSingleton.critical(root, qsTr("Event name must be specified."));
				ret = false;
			}
			else if(root.stageCount < 1) {
				MessageBoxSingleton.critical(root, qsTr("Stage count must be greater than zero."));
				ret = false;
			}
		}
		return ret;
	}
}
