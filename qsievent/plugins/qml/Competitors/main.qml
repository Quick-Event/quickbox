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
		NetworkAccessManager {
			id: networkAccessManager
		}
	}

	property list<Action> actions: [
		Action {
			id: actLoadFromOris
			text: qsTr('Load from Oris')
			onTriggered: {
				var reply = networkAccessManager.get('http://oris.orientacnisporty.cz/API/?format=json&method=getEventEntries&eventid=2526');
				reply.downloadProgress.connect(FrameWork.showProgress);
				reply.finished.connect(function(get_ok) {
					Log.info("http get finished:", get_ok, reply.url);
					if(get_ok) {
						var json_str = reply.textData;
						//Log.info("data:", json_str);
					}
					else {
						console.error("http get error:", reply.errorString, 'on:', reply.url)
					}
					reply.destroy();
				});
			}
		}
	]

	Component.onCompleted:
	{
		var act_file = FrameWork.menuBar.actionForPath('file');
		var act_competitors = act_file.appendMenu('competitors', qsTr('&Competitors'));
		FrameWork.menuBar.actionForPath('competitors/import').text = '&Import';
		FrameWork.menuBar.actionForPath('competitors/import').addAction(actLoadFromOris);
		//console.warn('featureId:', root.featureId);
		FrameWork.addPartWidget(thisPart, manifest.featureId);
	}

}
