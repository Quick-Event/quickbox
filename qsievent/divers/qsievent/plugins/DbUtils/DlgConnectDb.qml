import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgConnectDb"
	property var settings: Settings {}
	Frame {
		id: frm
		layoutType: bt.horizontal? Frame.LayoutHorizontal:  Frame.LayoutVertical
		Frame {
			Label {
				text: "ahoj"
			}
			Label {
				text: "bejby"
			}
		}
		Frame {
			Layout.rowSpan: 2

			LineEdit {
				text: "12345"
			}
			LineEdit {
				text: "6789"
			}
		}
	}
	Button {
		id: bt
		text: "change layout"
		property bool horizontal: true
		onClicked: {
			horizontal = !horizontal;
		}
	}
	Splitter {
		persistentSettingsId: "Splitter";
		//orientation: Qt.Vertical
		Label {
			text: "ahoj Splitter"
		}
		Label {
			text: "bejby Splitter"
		}
	}
	Component.onCompleted: {
		root.loadPersistentSettings(true);
	}
}
