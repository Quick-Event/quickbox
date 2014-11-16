import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import shared.QuickEvent 1.0

QuickEventPartWidget
{
	id: root

	title: "Start"

	Frame {
		Label {
			id: lbl
			text: "sprc my mrdka"
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
				objectName: "tvStart1"
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
						queryBuilder.select2('runners', '*')
							.select("lastName || ' ' || firstName AS name")
							.from('runners').orderBy('id');
					}
				}
			}
			TableView {
				id: classes
				objectName: "tvStart2"
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
		model.reload();
		mClasses.reload();
	}

}
