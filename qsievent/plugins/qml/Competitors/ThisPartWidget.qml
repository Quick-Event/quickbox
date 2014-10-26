import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

PartWidget
{
	id: root

	title: "Coompetitors"

	property QfObject internals: QfObject
	{
		Component {
			id: cDialog
			Dialog {
				persistentSettingsId: "DlgPrint"
			}
		}
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
	}

	Frame {
		layoutProperties: LayoutProperties { spacing: 0 }
		TableViewToolBar {
			id: tableViewToolBar
			tableView: table
		}
		TableView {
			id: table
			persistentSettingsId: "tblCompetitors";

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
					queryBuilder.select2('runners', 'id, classId, importId')
						.select("lastName || ' ' || firstName AS name")
						.from('runners').orderBy('id').limit(5);
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
			//tableViewToolBar.tableView = table;
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
	}

	function printAll()
	{
		Log.info("competitors print all triggered");
		var tt = new TreeTable.Table();
		tt.setData(model.toTreeTableData());
		//console.warn("tt1", tt.toString());
		tt.addColumn("test_col");
		for(var i=0; i<tt.rowCount(); i++) 
			tt.setValue(i, "test_col", "test_data_" + 1);
		var w = cReportViewWidget.createObject(null);
		w.setReport("/home/fanda/proj/quickbox/qsievent/plugins/qml/Competitors/reports/table.qml");
		//console.warn(tt.toString());
		w.setData(tt.data());
		var dlg = cDialog.createObject(FrameWork);
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}
}
