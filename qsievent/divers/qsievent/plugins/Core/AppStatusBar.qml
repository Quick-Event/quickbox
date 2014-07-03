import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

StatusBar
{
	id: root
	sizeGripEnabled: true
	property var eventFrame: Frame {
		Label {
			id: eventName
			text: "kkt"
		}
	}
	property var etapFrame: Label {
			text: "ETAP"
	}
	Component.onCompleted:
	{
		root.addPermanentWidget(eventFrame);
		root.addPermanentWidget(etapFrame);
	}
}
