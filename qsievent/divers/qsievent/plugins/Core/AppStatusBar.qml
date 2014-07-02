import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

StatusBar
{
	id: root
	sizeGripEnabled: true
	property Label eventName: Label {
		text: "kkt"
	}
	Component.onCompleted:
	{
		root.addPermanentWidget(eventName);
	}
}
