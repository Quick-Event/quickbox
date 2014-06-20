import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

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
