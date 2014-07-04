import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import qf.qmlwidgets.framework 1.0

PartWidget
{
	id: root

	title: "Start"

	Label {
		text: title
	}

	Component.onCompleted:
	{
		//FrameWork.menuBar.actionForPath('help').addAction(actLAboutQt);
		//FrameWork.menuBar.actionForPath('help').addSeparator();
	}

	function canActivate(active_on)
	{
		Log.info(title, "canActivate:", active_on);
		return true;
	}

}
