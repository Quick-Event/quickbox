import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	SettingsPersistence.key: "DlgConnectDb";
	property var settings: Settings {}
	Frame {
		layoutType: Frame.LayoutHorizontal
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
	Splitter {
		SettingsPersistence.key: "Splitter";
		//orientation: Qt.Vertical
		Label {
			text: "ahoj Splitter"
		}
		Label {
			text: "bejby Splitter"
		}
	}
}
