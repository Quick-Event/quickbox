import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		property LogDockWidget logDockWidget: null
		Component {
			id: cLogDockWidget
			LogDockWidget {}
		}

		property list<Action> actions: [
			Action {
				id: actShowLogView
				text: qsTr('Show application log')
				shortcut: "Ctrl+L"
				onTriggered: {
					Log.info(text, "triggered");
					showLogDockWidget(true);
				}
			},
			Action {
				id: actConfigureLogging
				text: qsTr('Logging')
				//shortcut: "Ctrl+L"
				onTriggered: {
					Log.info(text, "triggered");
					//showLogDockWidget(true);
				}
			}
		]
	}

	Component.onCompleted:
	{
		FrameWork.aboutToClose.connect(saveSettings);

		FrameWork.menuBar.actionForPath('tools/pluginSettings').addActionInto(actConfigureLogging);
		FrameWork.menuBar.actionForPath('view').addActionInto(actShowLogView);

		var core_feature = FrameWork.plugin("Core");
		var settings = core_feature.api.createSettings();
		settings.beginGroup("persistentSettings/ui/docks/Logger");
		var dock_visible = settings.value('visible');
		settings.destroy();
		//console.debug("logger dock visible:", dock_visible, typeof dock_visible);
		showLogDockWidget(dock_visible);
	}

	function saveSettings()
	{
		var core_feature = FrameWork.plugin("Core");
		var settings = core_feature.api.createSettings();
		settings.beginGroup("persistentSettings/ui/docks/Logger");
		settings.setValue('visible', (internals.logDockWidget)? internals.logDockWidget.visible: false);
		settings.destroy();
	}

	function showLogDockWidget(set_visible)
	{
		//console.debug("showLogDockWidget():", internals.logDockWidget);
		//console.trace();
		if(set_visible) {
			if(!internals.logDockWidget) {
				var w = cLogDockWidget.createObject(null);
				w.objectName = "logDockWidget";
				FrameWork.addDockWidget(Qt.BottomDockWidgetArea, w);
				internals.logDockWidget = w;
			}
			internals.logDockWidget.show();
		}
		else {
			if(internals.logDockWidget)
				internals.logDockWidget.hide();
		}
	}
}
