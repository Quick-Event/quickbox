import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Dialog {
	id: root
	persistentSettingsId: "DlgLayoutTest"
	//property var settings: Settings {}
	Label {
		text: "FormLayout"
	}
	Frame {
		layoutType: Frame.LayoutForm
		LineEdit {
			Layout.buddyText: "&edit with buddy"
			text: "10.0.0.3"
		}
		LineEdit {
			Layout.columnSpan: 2
			text: "column span"
		}
		Label {
			text: "addWidget"
		}
		LineEdit {
			text: "addWidget"
		}
	}
	Label {
		text: "GridLayout"
	}
	Frame {
		layoutType: Frame.LayoutGrid
		gridLayoutProperties: GridLayoutProperties {
			columns: 3
		}
		Label {
			text: "1st col"
		}
		Label {
			text: "2nd col"
		}
		Label {
			text: "3rd col"
		}
		LineEdit {
			Layout.columnSpan: 2
			text: "2 column span"
		}
		LineEdit {
			text: "3rd column"
		}
		LineEdit {
			text: "1st column"
		}
		LineEdit {
			Layout.columnSpan: 2
			text: "2 column span"
		}
		LineEdit {
			Layout.columnSpan: 3
			text: "3 column span"
		}
	}
	Label {
		text: "FrameLayout"
	}
	Frame {
		id: frm
		objectName: "my frame"
		layoutType: bt.horizontal? Frame.LayoutHorizontal:  Frame.LayoutVertical
		Frame {
			Label {
				text: "foo"
			}
			Label {
				text: "bar"
			}
		}
		Frame {
			Layout.rowSpan: 2

			LineEdit {
				text: "baz"
			}
			LineEdit {
				text: "faz"
			}
		}
	}
	Button {
		id: bt
		text: "change layout"
		property bool horizontal: false
		onClicked: {
			horizontal = !horizontal;
		}
	}
	Label {
		text: "Splitter"
	}
	Splitter {
		persistentSettingsId: "Splitter";
		//orientation: Qt.Vertical
		Label {
			text: "hello Splitter"
		}
		Label {
			text: "hi Splitter"
		}
	}
	Component.onCompleted: {
		root.loadPersistentSettings(true);
	}
}
