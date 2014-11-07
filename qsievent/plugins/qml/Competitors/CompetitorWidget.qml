import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DialogWidget
{
	id: root
	property string pluginId
	objectName: "dwCompetitors"
	title: "Competitor"
	
	iconSource: {
		var plugin = FrameWork.plugin(root.pluginId)
		//console.debug("@plugin:", plugin)
		// console.debug("#manifest:", plugin.manifest)
		// console.debug("$homeDirectory:", plugin.manifest.homeDirectory)
		return plugin.manifest.homeDirectory + "/images/feature.png"
	}
	
	attachedObjects: [
	]

	Frame {
		Label {
			text: "Ahoj"
		}
	}

	Component.onCompleted: {
		console.debug("-----------------------onCompleted---------------")
		console.debug("@iconSource:", root.iconSource)
	}
}
