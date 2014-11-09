import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DialogWidget
{
	id: root
	objectName: "dwCompetitors"
	title: "Competitor"

	iconSource: {
		var plugin = FrameWork.plugin("Competitors")
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
