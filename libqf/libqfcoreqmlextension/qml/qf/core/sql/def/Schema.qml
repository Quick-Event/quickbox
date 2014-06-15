import qf.core
import qf.core.sql.def

QtObject {
	property string name
    property list<Table> tables

	function createSQLCommand(server_type, options)
	{
		var ret = [];
		ret.push('-- script generated to create schema ' + name);
		for(var i=0; i<tables.length; i++) {
			ret.push.apply(ret, tables[i].createSQLCommand(server_type, options));
		}
		return ret.join('\n');
	}
}