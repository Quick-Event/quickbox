import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import shared.QuickEvent 1.0

QuickEventPartWidget
{
	id: root

	title: "Start"

	Frame {
		/*
		Label {
			id: lbl
			text: "hello"
		}
		Button {
			text: "format C:"
			onClicked: {
				lbl.text = "kkt"
			}
		}
		*/
		TableView {
			id: table
			objectName: "tvStart1"
			persistentSettingsId: "tblStart";

			model: SqlTableModel {
				id: model
				ModelColumn {
					fieldName: 'id'
				}
				ModelColumn {
					fieldName: 'classes.name'
					caption: qsTr('class')
				}
				ModelColumn {
					fieldName: 'competitorName'
					caption: qsTr('Name')
				}
				ModelColumn {
					fieldName: 'importId'
				}
				Component.onCompleted:
				{
					queryBuilder
						.select2('competitors', '*')
						.select2('classes', 'name')
						.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
						.from('competitors').orderBy('competitors.id')
						.join("competitors.classId", "classes.id");
				}
			}
		}
	}

	function reload()
	{
		model.reload();
	}

}
