import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DialogWidget
{
	id: root
	objectName: "dwCompetitors"
	title: "Competitor"

	iconSource: "images/feature.png"

	attachedObjects: [
	]

	Frame {
		layoutType: Frame.LayoutGrid
		layoutProperties: LayoutProperties {
			columns: 4
		}

		Label {
			text: qsTr("Class")
		}
		LineEdit {
			text: "H21E"
		}
		Label {
			horizontalAlignment: Qt.AlignRight
			text: qsTr("SI")
		}
		LineEdit {
			text: "8436"
		}

		Label {
			text: qsTr("First name")
		}
		LineEdit {
			Layout.columnSpan: 3
			text: "Fanda"
		}
		Label {
			text: qsTr("Last name")
		}
		LineEdit {
			Layout.columnSpan: 3
			text: "Vacek"
		}

		Label {
			text: qsTr("Registration")
		}
		LineEdit {
			text: "CHT7001"
		}
		Label {
			text: qsTr("Licence")
		}
		LineEdit {
			text: "C"
		}

		Label {
			text: qsTr("Club")
		}
		LineEdit {
			text: "SKOB Chrast"
		}
		Label {
			text: qsTr("Country")
		}
		LineEdit {
			text: "Czech republic"
		}

		Label {
			Layout.verticalSizePolicy: LayoutProperties.Expanding
			text: "REST"
		}
	}

	Component.onCompleted: {
		console.debug("-----------------------onCompleted---------------")
		console.debug("@iconSource:", root.iconSource)
	}
}
