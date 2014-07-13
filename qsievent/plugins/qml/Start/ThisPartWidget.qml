import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

PartWidget
{
	id: root

	title: "Start"

	Frame {
		Label {
			text: "sprc my mrdka"
		}
		Frame {
			layoutType: Frame.LayoutHorizontal
			TableView {
				id: table
				model: SqlQueryTableModel {
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
							.select("firstName || ' ' || lastName AS name")
							.from('runners').orderBy('id');
					}
				}
			}
			TableView {
				id: classes
				model: SqlQueryTableModel {
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

	Component.onCompleted:
	{
	}

	function canActivate(active_on)
	{
		Log.info(title, "canActivate:", active_on);
		if(active_on) {
			model.reload();
			mClasses.reload();
		}
		return true;
	}

}
