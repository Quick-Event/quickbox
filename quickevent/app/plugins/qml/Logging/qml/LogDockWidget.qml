import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Logging 1.0

DockWidget
{
	id: root

	objectName: "LogDockWidget"
	windowTitle: "Application log"

	LoggerWidget {}

	Component.onCompleted:
	{
		Log.info("Component.onCompleted");
	}

}
