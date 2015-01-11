import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import shared.QuickEvent 1.0
import Core 1.0

QuickEventPartWidget
{
	id: root

	title: "Start"
	Frame {
		layoutType: Frame.LayoutHorizontal
		Label {
			text: qsTr("Class")
		}
		ForeignKeyComboBox {
			id: classList
			referencedTable: "classes"
			referencedField: "id"
			referencedCaptionField: "name"

			function checkLoaded()
			{
				if(classList.count == 0) {
					loadItems();
					insertItem(0, qsTr("--- all ---"), 0);
					currentDataChanged.connect(root.reload)
				}
			}
		}
		Label {
			Layout.horizontalSizePolicy: LayoutProperties.Expanding
			text: qsTr("<--->")
		}
	}
	Frame {
		layoutProperties: LayoutProperties { spacing: 0 }
		attachedObjects: [
		/*
			OBItemDelegate {
				id: obItemDelegate
			}
		*/
		]
		TableViewToolBar {
			id: tableViewToolBar
			tableView: table
		}
		TableView {
			id: table
			objectName: "tvStart1"
			persistentSettingsId: "tblStart";

			model: OGSqlTableModel {
				id: model
				ModelColumn {
					fieldName: 'competitors.id'
					readOnly: true
					caption: qsTr('ID')
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
					fieldName: 'stageId'
					caption: qsTr('Stage')
				}
				ModelColumn {
					fieldName: 'startTimeMS'
					caption: qsTr('Start')
				}
				Component.onCompleted:
				{
					queryBuilder
						.select2('competitors', '*')
						.select2('classes', 'name')
						.select2('laps', '*')
						//.select2('laps', 'stageId, startTimeMS')
						.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
						.from('competitors').orderBy('competitors.id')
						.join("competitors.classId", "classes.id")
						.joinRestricted("competitors.id", "laps.competitorId", "stageId=1");//.where("competitors.id > 370");
				}
			}
			Component.onCompleted:
			{
				//setItemDelegateForColumn(4, obItemDelegate);
			}
		}
	}

	function reload()
	{
		classList.checkLoaded();
		model.queryBuilder.clearWhere();
		var class_id = classList.currentData;
		if(class_id)
			model.queryBuilder.where("competitors.classId=" + class_id);
		Log.info("reloading class id:", class_id)
		model.reload();
	}

}
