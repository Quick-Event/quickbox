import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0
import "private/libsqldef.js" as SqlDef

QtObject {
	id: root
	property string name
	property list<Field> fields
	property list<Index> indexes
	property var rows: [] // list of values list
	property string comment

	function createSqlScript(options)
	{
		var opts = new SqlDef.Options(options);
		var ret = [];
		var sql_types = [];
		var full_table_name = root.name;
		if(options.driverName.endsWith("PSQL"))
			full_table_name = options.schemaName + '.' + root.name;
		ret.push('-- create table: ' + full_table_name);
		var table_def = 'CREATE TABLE ' + full_table_name + ' (\n';
		var field_defs = [];
		options.tableName = name;
		var fieldNames = []
		for(var i=0; i<fields.length; i++) {
			var field = fields[i];
			fieldNames.push(field.name);
			//Log.info("field:", field);
			field_defs.push(field.createSqlScript(options));
			var sql_type = field.createTypesSqlScript(options);
			if(sql_type)
				sql_types.push(sql_type);
		}
		for(var i=0; i<indexes.length; i++) {
			var constr = indexes[i].createSqlConstraintScript(i, options);
			if(constr)
				field_defs.push(constr);
		}
		table_def += field_defs.join(',\n');
		table_def += '\n)';

		ret.push.apply(ret, sql_types);

		ret.push(table_def);

		for(var i=0; i<indexes.length; i++) {
			var index = indexes[i];
			var index_def = index.createSqlIndexScript(options);
			if(index_def)
				ret.push('CREATE INDEX ' + index.indexName(i, name) + ' ON ' + full_table_name + ' ' + index_def);
		}
		var comments_prefix = "";
		if(!opts.isTableCommentsSupported()) {
			comments_prefix = "-- comments not suported for driver: " + options.driverName + "\n-- ";
		}
		for(var i=0; i<fields.length; i++) {
			var fld = fields[i];
			if(fld.comment) {
				ret.push(comments_prefix + 'COMMENT ON COLUMN ' + full_table_name + '.' + fld.name + " IS '" + fld.comment + "'");
			}
		}
		if(root.comment) {
			ret.push(comments_prefix + 'COMMENT ON TABLE ' + full_table_name + " IS '" + root.comment + "'");
		}
		var ins = "INSERT INTO " + full_table_name + " (" + fieldNames.join(", ") + ") VALUES "
		for(var i=0; i<root.rows.length; i++) {
			var row = root.rows[i];
			var vals = ""
			for(var j=0; j<row.length; j++) {
				if(j > 0)
					vals += ', ';
				vals += quoteSqlValue(row[j], options);
			}
			ret.push(ins + "(" + vals + ")");
		}
		return ret;
	}

	function isString(o)
	{
	    return typeof o == "string" || (typeof o == "object" && o.constructor === String);
	}

	function quoteSqlValue(val, options)
	{
		var ret;
		if(typeof val === 'undefined')
			ret = 'NULL';
		else if(val === null)
			ret = 'NULL';
		else if(isString(val))
			ret = "'" + val + "'";
		else if(isNaN(val)) {
			if(options.driverName.endsWith("PSQL"))
				ret = 'DEFAULT';
			else
				ret = 'NULL'
		}
		else
			ret = val + "";
		return ret;
	}
}
