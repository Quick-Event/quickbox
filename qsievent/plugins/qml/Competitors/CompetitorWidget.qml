import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
// import "qrc:/qf/core/qml/js/treetable.js" as TreeTable

DialogWidget
{
	id: root
	property var plugin
	objectName: "dwCompetitors"
	title: "Competitor"
	iconSource: plugin.manifest.homeDirectory + "/images/feature.id"

	attachedObjects: [
	]

	Frame {
		Label {
			text: "Ahoj"
		}
	}
}
