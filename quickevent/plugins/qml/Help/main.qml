import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		Component {
			id: dlgLayoutTest
			DlgLayoutTest {}
		}
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
		Component {
			id: cDialog
			Dialog {}
		}
	}

	property list<Action> actions: [
		Action {
			id: actLAboutQt
			text: qsTr('About &Qt')
			onTriggered: {
				MessageBoxSingleton.aboutQt();
			}
		},
		Action {
			id: actLayoutTest
			text: qsTr('&Layout test')
			onTriggered: {
				Log.info(text, "triggered");
				var dlg = dlgLayoutTest.createObject(FrameWork);
				dlg.exec();
				dlg.destroy();
			}
		},
		Action {
			id: actReportTest
			text: qsTr('&Report test')
			onTriggered: {
				Log.info(text, "triggered");
				var dlg = dlgLayoutTest.createObject(FrameWork);
				var w = cReportViewWidget.createObject(null);
				w.windowTitle = qsTr("Report test");
				w.setReport(homeDir() + "/reports/test02.qml");
				//console.warn("setting data:", tt.toString());
				//w.setData(tt.data());
				var dlg = cDialog.createObject(FrameWork);
				dlg.setDialogWidget(w);
				dlg.exec();
				dlg.destroy();
			}
		},
		Action {
			id: actGC
			text: qsTr('Collect garbage')
			onTriggered: {
				Log.info(text, "triggered");
				gc();
			}
		}
	]

	Component.onCompleted:
	{
		var act_help = FrameWork.menuBar.actionForPath('help');
		act_help.addActionInto(actLAboutQt);
		act_help.addSeparatorInto();
		act_help.addActionInto(actLayoutTest);
		act_help.addActionInto(actReportTest);
		act_help.addSeparatorInto();
		act_help.addActionInto(actGC);
	}

}
