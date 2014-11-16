import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DataDialogWidget
{
	id: root
	objectName: "dwCompetitors"
	title: "Competitor"

	iconSource: "images/feature.png"

	dataController: DataController {
		widget: root
		document: SqlDataDocument {
			id: dataDocument
			Component.onCompleted:
			{
				queryBuilder.select2('runners', '*')
					.select("lastName || ' ' || firstName AS name")
					.from('runners').where('runners.id={{ID}}');
			}
			onLoaded: {
				root.title = dataDocument.value("name");
			}
		}
	}

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
			dataId: "runners.classId"
		}
		Label {
			horizontalAlignment: Qt.AlignRight
			text: qsTr("SI")
		}
		LineEdit {
			dataId: "runners.siId"
		}

		Label {
			text: qsTr("First name")
		}
		LineEdit {
			Layout.columnSpan: 3
			dataId: "runners.firstName"
			
		}
		Label {
			text: qsTr("Last name")
		}
		LineEdit {
			Layout.columnSpan: 3
			dataId: "runners.lastName"
		}

		Label {
			text: qsTr("Registration")
		}
		LineEdit {
			dataId: "runners.registration"
		}
		Label {
			text: qsTr("Licence")
		}
		LineEdit {
			dataId: "runners.licence"
		}

		Label {
			text: qsTr("Club")
		}
		LineEdit {
			dataId: "runners.club"
		}
		Label {
			text: qsTr("Country")
		}
		LineEdit {
			dataId: "runners.country"
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
