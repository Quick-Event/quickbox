import QtQml 2.0

QtObject {
	id: root
	property string featureId // by default same as plugin directory
	property var dependsOnFeatureIds
	property string pluginLoader // by default, main.qml
	//property string visualSlot
	property bool disabled: false
}
