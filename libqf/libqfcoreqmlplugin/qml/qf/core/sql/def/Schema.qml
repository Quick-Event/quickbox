import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt

QtObject {
	id: root
	property string name
    property list<Table> tables

	function createSqlScript(options)
	{
		name = options.schemaName;
		var driver_name = options.driverName;
		var ret = [];
		ret.push('-- script generated to create schema ' + name);
		if(driver_name.endsWith("PSQL"))
			ret.push('CREATE SCHEMA ' + name);
		for(var i=0; i<tables.length; i++) {
			ret.push.apply(ret, tables[i].createSqlScript(options));
		}
		return ret;//.join(';\n');
	}
}
