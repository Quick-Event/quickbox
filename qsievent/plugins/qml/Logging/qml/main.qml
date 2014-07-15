import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property list<Action> actions: [
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
		FrameWork.aboutToClose.connect(saveSettings);

		FrameWork.menuBar.actionForPath('view').addAction(actShowLogView);

		var core_feature = FrameWork.plugin("Core");
		var settings = core_feature.createSettings();
		settings.beginGroup("persistentSettings/ui/docks/Logger");
		var dock_visible = settings.value('visible');
		settings.destroy();
		//console.debug("logger dock visible:", dock_visible, typeof dock_visible);
		if(dock_visible) {
			showLogDockWidget();
		}
	}

	function saveSettings()
	{
		var core_feature = FrameWork.plugin("Core");
		var settings = core_feature.createSettings();
		settings.beginGroup("persistentSettings/ui/docks/Logger");
		settings.setValue('visible', (internals.logDockWidget)? internals.logDockWidget.visible: false);
		settings.destroy();
	}

	function showLogDockWidget()
	{
		//console.debug("showLogDockWidget():", internals.logDockWidget);
		//console.trace();
		if(!internals.logDockWidget) {
			internals.logDockWidget = logDockWidgetComponent.createObject(null);
			FrameWork.addDockWidget(Qt.BottomDockWidgetArea, internals.logDockWidget);
		}
		internals.logDockWidget.show();
	}
}
