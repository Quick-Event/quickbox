import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

StatusBar
{
	id: root

	property string eventName: '---'
	property int etapNo: 0

	sizeGripEnabled: true
	property var eventFrame: Frame {
		frameShape: QFrame.Box
		frameShadow: QFrame.Sunken
		Label {
			id: eventName
			text: root.eventName
		}
	}
	property var etapFrame: Frame {
		frameShape: QFrame.Box
		frameShadow: QFrame.Sunken
		visible: (root.etapNo > 0)
		Label {
			id: etap
			text: (root.etapNo>0)? ('E'+root.etapNo): ''
		}
	}
	/*
	property var pluginEvent: null
	function whenCurrentEventNameChanged()
	{
		eventName.text = pluginEvent.currentEventName;
	}
	function lazyInit()
	{
		pluginEvent = FrameWork.plugin('Event');
		pluginEvent.currentEventNameChanged.connect(whenCurrentEventNameChanged);
	}
	*/
	Component.onCompleted:
	{
		root.addPermanentWidget(eventFrame);
		root.addPermanentWidget(etapFrame);

		//FrameWork.pluginsLoaded.connect(lazyInit);
	}
}
