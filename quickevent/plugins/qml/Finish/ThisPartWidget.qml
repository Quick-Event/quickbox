import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import shared.QuickEvent 1.0

QuickEventPartWidget
{
	id: root
	objectName: "pwFinish"
	title: "Finish"

	Frame {
		Label {
			id: lbl
			text: "tATRY"
		}
		Button {
			text: "format C:"
			onClicked: {
				lbl.text = "kkt"
			}
		}
		Frame {
			layoutType: Frame.LayoutHorizontal
			TableView {
				id: table
				objectName: "twFinish01"
				persistentSettingsId: "tblStart";

				model: SqlTableModel {
					id: model
					ModelColumn {
						fieldName: 'id'
					}
					ModelColumn {
						fieldName: 'classId'
						caption: qsTr('class')
					}
					ModelColumn {
						fieldName: 'name'
						caption: qsTr('Name')
					}
					ModelColumn {
						fieldName: 'importId'
					}
					Component.onCompleted:
					{
						queryBuilder.select2('competitors', '*')
							.select("lastName || ' ' || firstName AS name")
							.from('competitors').orderBy('id');
					}
				}
			}
			TableView {
				id: classes
				objectName: "twFinish02"
				persistentSettingsId: "tblClasses";
				model: SqlTableModel {
					id: mClasses
					Component.onCompleted:
					{
						queryBuilder.select2('classes', '*')
							.from('classes').orderBy('id');
					}
				}
			}
		}
	}

	function reload()
	{
		Log.warning("Reloading Finish tables")
		model.reload();
		mClasses.reload();
	}

}
