import QtQml 2.0
import qf.core 1.0
import "private/libsqldef.js" as LibSqlDef

QtObject {
	id: root
	property string name
	property list<Field> fields
	property list<Index> indexes
	property string comment

	function createSqlScript(options)
	{
		var opts = new LibSqlDef.Options(options);
		var ret = [];
		var sql_types = [];
		var full_table_name = opts.fullTableName(name);
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
		return ret;
	}
}
