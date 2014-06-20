import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

QtObject {
	property string name
	property list<Field> fields
	property list<Index> indexes

	function createSQLCommand(server_type, options)
	{
		var ret = '-- create table: ' + name;
		return ret;
	}
}
