import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
import shared.QuickEvent 1.0

QuickEventPartWidget
{
	id: root
	objectName: "pwCompetitors"
	title: "Competitors"

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
			id: cCompetitorWidget
			CompetitorWidget {
			}
		}
	]

	Frame {
		layoutProperties: LayoutProperties { spacing: 0 }
		TableViewToolBar {
			id: tableViewToolBar
			tableView: table
		}
		TableView {
			id: table
			persistentSettingsId: "tblCompetitors";
			rowEditorMode: TableView.EditRowsMixed
			inlineEditStrategy: TableView.OnCurrentFieldChange

			model: SqlTableModel {
				id: model
				ModelColumn {
					fieldName: 'id'
					readOnly: true
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
					fieldName: 'registration'
					caption: qsTr('Reg')
				}
				ModelColumn {
					fieldName: 'siId'
					caption: qsTr('SI')
				}
				Component.onCompleted:
				{
					queryBuilder.select2('competitors', '*')
						.select2('classes', 'name')
						.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
						.from('competitors')
						.join("competitors.classId", "classes.id")
						.orderBy('competitors.id').limit(100);
				}
			}
		}
	}

	Component.onCompleted:
	{
		table.editRowInExternalEditor.connect(editCompetitor)
	}

	function reload()
	{
		Log.info("Reloading competitors model");
		model.reload();
	}

	function printAll()
	{
		Log.info("competitors print all triggered");
		var tt = new TreeTable.Table();
		tt.setData(model.toTreeTableData());
		tt.setValue("title", "Competitors list")
		//console.warn("tt1", tt.toString());
		tt.addColumn("test_col");
		for(var i=0; i<tt.rowCount(); i++)
			tt.setValue(i, "test_col", "test_data_" + 1);
		var w = cReportViewWidget.createObject(null);
		w.windowTitle = qsTr("Competitors");
		w.setReport(pluginHomeDir() + "/reports/list.qml");
		//console.warn("setting data:", tt.toString());
		w.setData(tt.data());
		var dlg = cDialog.createObject(FrameWork);
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}

	function editCompetitor(id, mode)
	{
		Log.info("editCompetitor id:", id, "mode:", mode);
		var w = cCompetitorWidget.createObject(null);
		w.windowTitle = qsTr("Edit Competitor");
		var dlg = cDialog.createObject(root);
		dlg.setDialogWidget(w);
		w.load(id, mode);
		w.dataSaved.connect(table.rowExternallySaved);
		dlg.exec();
		dlg.destroy();
	}

}
