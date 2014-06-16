import qf.core
import qf.core.sql.def

QtObject {
	property string name
	property list<Field> fields
	property list<Index> indices

	function createSQLCommand(server_type, options)
	{
		var ret = '-- create table: ' + name;
		return ret;
	}
}