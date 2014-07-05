import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

StatusBar
{
	id: root

	property string eventName: '---'
	property int etapNo: 0

	styleSheet: "background-color: rgb(118, 118, 118);"
	sizeGripEnabled: true
	property var eventFrame: Frame {
		frameShape: QFrame.Panel
		frameShadow: QFrame.Sunken
		Label {
			id: eventName
			text: root.eventName
		}
	}
	property var etapFrame: Frame {
		frameShape: QFrame.Panel
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
