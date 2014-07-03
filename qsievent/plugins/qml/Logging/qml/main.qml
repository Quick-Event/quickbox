import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

Plugin {
	id: root

	actions: [
		Action {
			id: actShowLogView
			text: qsTr('Show application log')
			shortcut: "Ctrl+L"
			//enabled: internals.pluginSqlDb.sqlServerConnected
			//checkable: true
			onTriggered: {
				Log.info(text, "triggered");
				showLogDockWidget();
			}
		}
	]

	property QfObject internals: QfObject
	{
		property LogDockWidget logDockWidget: null
		Component {
			id: logDockWidgetComponent
			LogDockWidget {}
		}
	}

	Component.onCompleted:
	{
		FrameWork.menuBar.actionForPath('view').addAction(actShowLogView);
	}

	function showLogDockWidget()
	{
		if(!internals.logDockWidget) {
			internals.logDockWidget = logDockWidgetComponent.createObject(null);
			FrameWork.addDockWidget(Qt.BottomDockWidgetArea, internals.logDockWidget);
		}
		internals.logDockWidget.show();
	}
}
