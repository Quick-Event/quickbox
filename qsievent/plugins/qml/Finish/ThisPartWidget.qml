import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

PartWidget
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
				objectName: "twFinish02"
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
		FrameWork.plugin("SqlDb").api.onSqlServerConnectedChanged.connect(reloadIfActive);
		FrameWork.plugin("Event").api.onCurrentEventNameChanged.connect(reloadIfActive);
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
		Log.warning("Reloading Finish tables")
		var sql_connected = FrameWork.plugin("SqlDb").api.sqlServerConnected;
		var event_name = FrameWork.plugin("Event").api.currentEventName;
		if(!sql_connected || !event_name)
			return;
		model.reload();
		mClasses.reload();
	}

}
