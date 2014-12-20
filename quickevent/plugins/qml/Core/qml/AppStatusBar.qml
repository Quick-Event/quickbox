import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

StatusBar
{
	id: root

	property string eventName: '---'
	property int stageNo: 0

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
	property var stageFrame: Frame {
		frameShape: QFrame.Panel
		frameShadow: QFrame.Sunken
		visible: (root.stageNo > 0)
		Label {
			id: stage
			text: (root.stageNo>0)? ('E'+root.stageNo): ''
		}
	}
	property var progressFrame: Frame
	{
		visible: false
		layoutType: Frame.LayoutHorizontal
		ProgressBar
		{
			id: progressBar
			autoFillBackground: true
			minimum: 0
			textVisible: true
			minimumWidth: 200
			maximumWidth: minimumWidth
		}
		Label {
			id: progressBarLabel
		}
		function showProgress(msg, completed, total)
		{
			//console.warn(msg, completed, total);
			visible = (completed < total);
			progressBar.value = completed;
			progressBar.maximum = total;
			progressBarLabel.text = msg;
		}
	}
	Component.onCompleted:
	{
		root.addPermanentWidget(eventFrame);
		root.addPermanentWidget(stageFrame);

		root.addWidget(progressFrame);

		FrameWork.progress.connect(progressFrame.showProgress);
	}
}
