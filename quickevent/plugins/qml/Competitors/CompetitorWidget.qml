import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DataDialogWidget
{
	id: root
	persistentSettingsId: "CompetitorDetail"
	objectName: "dwCompetitors"
	title: "Competitor"

	iconSource: "images/feature.png"

	dataController: DataController {
		widget: root
		document: SqlDataDocument {
			id: dataDocument
			Component.onCompleted:
			{
				queryBuilder.select2('competitors', '*')
					.select("lastName || ' ' || firstName AS name")
					.from('competitors').where('competitors.id={{ID}}');
			}
			onLoaded: {
				var name = dataDocument.value("name");
				if(name)
					root.title = name;
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
		ForeignKeyComboBox {
			id: edClass
			dataId: "competitors.classId"
			referencedTable: "classes"
			referencedField: "id"
		}
		Label {
			horizontalAlignment: Qt.AlignRight
			text: qsTr("SI")
		}
		LineEdit {
			dataId: "competitors.siId"
		}

		Label {
			text: qsTr("First name")
		}
		LineEdit {
			Layout.columnSpan: 3
			dataId: "competitors.firstName"
			
		}
		Label {
			text: qsTr("Last name")
		}
		LineEdit {
			Layout.columnSpan: 3
			dataId: "competitors.lastName"
		}

		Label {
			text: qsTr("Registration")
		}
		LineEdit {
			dataId: "competitors.registration"
		}
		Label {
			text: qsTr("Licence")
		}
		LineEdit {
			dataId: "competitors.licence"
		}

		Label {
			text: qsTr("Club")
		}
		LineEdit {
			dataId: "competitors.club"
		}
		Label {
			text: qsTr("Country")
		}
		LineEdit {
			dataId: "competitors.country"
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

	// c++ override
	function dialogDoneRequest_qml(result)
	{
		Log.info("dialogDoneRequest result:", result);
		// call C++ impl, QML cannot call superclass implementation
		return dialogDoneRequest(result);
	}

	onVisibleChanged: {
		Log.info("onVisibleChanged visible:", visible);
		if(visible)
			edClass.setFocus();
	}
}
