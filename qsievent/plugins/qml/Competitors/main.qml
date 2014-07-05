import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Plugin {
	id: root

	property QfObject internals: QfObject
	{
		ThisPartWidget{
			id: thisPart
		}
	}

	property list<Action> actions: [
		Action {
			id: actLoadFromOris
			text: qsTr('Load from Oris')
			onTriggered: {
				Log.debug(text);
			}
		}
	]

	Component.onCompleted:
	{
		var act_file = FrameWork.menuBar.actionForPath('file');
		var act_competitors = act_file.appendMenu('competitors', qsTr('Competitors'));
		FrameWork.menuBar.actionForPath('competitors/import').addAction(actLoadFromOris);
		//console.warn('featureId:', root.featureId);
		FrameWork.addPartWidget(thisPart, manifest.featureId);
	}

}
