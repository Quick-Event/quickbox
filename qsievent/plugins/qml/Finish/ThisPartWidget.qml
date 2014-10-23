import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

PartWidget
{
	id: root

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
				persistentSettingsId: "tblStart";

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
							.select("lastName || ' ' || firstName AS name")
							.from('runners').orderBy('id');
					}
				}
			}
			TableView {
				id: classes
				persistentSettingsId: "tblClasses";
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
		FrameWork.plugin("SqlDb").onSqlServerConnectedChanged.connect(reloadIfActive);
		FrameWork.plugin("Event").onCurrentEventNameChanged.connect(reloadIfActive);
	}

	function canActivate(active_on)
	{
		console.debug(title, "canActivate:", active_on);
		if(active_on) {
			reload();
		}
		return true;
	}

	function reloadIfActive()
	{
		if(root.active)
			reload();
	}

	function reload()
	{
		var sql_connected = FrameWork.plugin("SqlDb").sqlServerConnected;
		var event_name = FrameWork.plugin("Event").currentEventName;
		if(!sql_connected || !event_name)
			return;
		model.reload();
		mClasses.reload();
	}

}
