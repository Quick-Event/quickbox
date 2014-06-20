import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

QtObject {
	property string name
    property var fields: []
    property bool primary: false
    property bool unique: false
}
