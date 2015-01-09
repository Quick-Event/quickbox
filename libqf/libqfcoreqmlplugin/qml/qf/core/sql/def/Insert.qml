import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0
import "private/libsqldef.js" as LibSqlDef

QtObject {
	id: root
	property Table table
	property var fields: [] // list of fields to insert
	property var rows: [] // list of values list to insert
	property string comment

	function createSqlScript(options)
	{
		var opts = new LibSqlDef.Options(options);
		var ret = [];
		var full_table_name = opts.fullTableName(table.name);

		ret.push('-- insert into table: ' + full_table_name);
		if(root.comment)
			ret.push('-- ' + root.comment);

		var fieldNames = root.fields
		if(fieldNames.length == 0) {
			for(var i=0; i<table.fields.length; i++) {
				var field = table.fields[i];
				fieldNames.push(field.name);
				//Log.info("field:", field);
			}
		}

		var values = []
		for(var i=0; i<root.rows.length; i++) {
			var row = root.rows[i];
			var vals = []
			for(var j=0; j<row.length; j++) {
				vals.push(opts.quoteSqlValue(row[j]));
			}
			values.push("(" + vals.join(", ") + ")")
		}
		ret.push("INSERT INTO " + full_table_name + " (" + fieldNames.join(", ") + ") VALUES \n" + values.join(",\n"))
		return ret;
	}
}
