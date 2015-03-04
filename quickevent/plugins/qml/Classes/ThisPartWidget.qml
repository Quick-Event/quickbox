import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
import shared.QuickEvent 1.0

QuickEventPartWidget
{
	id: root
	title: "Classes"

	attachedObjects: [
		Component {
			id: cDialog
			Dialog {
				//persistentSettingsId: "DlgPrint"
				buttonBox: ButtonBox {}
			}
		},
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		},
		Component {
			id: cClassWidget
			ClassWidget {
			}
		},
		Component {
			id: cSqlTableModel
			SqlTableModel {
				ModelColumn {
					fieldName: 'id'
					caption: qsTr('ID')
					readOnly: true
				}
				ModelColumn {
					fieldName: 'name'
					caption: qsTr('Name')
				}
				ModelColumn {
					fieldName: 'courses.name'
					caption: qsTr('Course')
				}
				Component.onCompleted:
				{
					queryBuilder
						.select2('classes', '*')
						.select2('courses', 'name')
						.from('classes')
						.join("classes.courseId", "courses.id")
						.orderBy('classes.name');
				}
			}
		}
	]

	Frame {
		layoutProperties: LayoutProperties { spacing: 0 }
		TableViewToolBar {
			id: tableViewToolBar
			tableView: tableView
		}
		TableView {
			id: tableView
			persistentSettingsId: "tblClasses";
			rowEditorMode: TableView.EditRowsMixed
			//idColumnName: "classId"

			model: cSqlTableModel.createObject(tableView);
		}
	}

	Component.onCompleted:
	{
		tableView.editRowInExternalEditor.connect(editClass)
	}

	function reload()
	{
		tableView.model.reload();
	}

	function printAll()
	{
		Log.info("Classes print all triggered");

		var model = cSqlTableModel.createObject();
		model.reload();
		var tt = new TreeTable.Table();
		tt.setData(model.toTreeTableData());
		model.destroy();

		//console.warn("tt1", tt.toString());
		tt.addColumn("test_col");
		for(var i=0; i<tt.rowCount(); i++)
			tt.setValue(i, "test_col", "test_data_" + 1);
		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Classes");
		//console.warn("setting report:", pluginHomeDir() + "/reports/table.qml");
		w.setReport(pluginHomeDir() + "/reports/table.qml");
		//console.warn("setting data:", tt.toString());
		w.setData(tt.data());
		var dlg = cDialog.createObject(FrameWork);
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}

	function editClass(id, mode)
	{
		Log.info("editClass id:", id, "mode:", mode);
		var w = cClassWidget.createObject(null);
		w.windowTitle = qsTr("Edit Class");
		var dlg = cDialog.createObject(root);
		dlg.setDialogWidget(w);
		w.load(id, mode);
		w.dataSaved.connect(tableView.rowExternallySaved);
		dlg.exec();
		dlg.destroy();
	}

}
