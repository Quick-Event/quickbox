import QtQml 2.0
import qf.core 1.0
import "private"

QtObject {
	property string name
	property FieldType type
	property int length: -1
	property string charset: "utf8"
	property var defaultValue
	property bool notNull: false
	property string comment
}
