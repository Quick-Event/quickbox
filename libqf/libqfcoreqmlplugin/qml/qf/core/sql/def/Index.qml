import qf.core
import qf.core.sql.def

QtObject {
	property string name
    property list<string> fields
    property bool primary: false
    property bool unique: false
}