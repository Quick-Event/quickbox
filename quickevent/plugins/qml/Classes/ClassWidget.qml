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
				queryBuilder.select2('classes', '*')
					.from('classes').where("classes.id='{{ID}}'");
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
			columns: 2
		}

		Label {
			text: qsTr("Class Id")
		}
		LineEdit {
			id: edClassId
			dataId: "classes.id"
		}
		Label {
			text: qsTr("Name")
		}
		LineEdit {
			id: edClassName
			dataId: "classes.name"
		}
		Label {
			text: qsTr("Course")
		}
		ForeignKeyComboBox {
			id: edCourse
			dataId: "classes.courseId"
			referencedTable: "courses"
			referencedField: "id"
			referencedCaptionField: "name"
		}
		Frame {}
	}

	Component.onCompleted: {
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
			edClassName.setFocus();
	}
}
