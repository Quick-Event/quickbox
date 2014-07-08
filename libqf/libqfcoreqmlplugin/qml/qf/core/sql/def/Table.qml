import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

QtObject {
	id: root
	property string name
	property list<Field> fields
	property list<Index> indexes
	property string comment

	function createSqlScript(options)
	{
		var ret = [];
		var sql_types = [];
		ret.push('-- create table: ' + options.schemaName + '.' + root.name);
		var table_def = 'CREATE TABLE ' + options.schemaName + '.' + root.name + ' (\n';
		var field_defs = [];
		options.tableName = name;
		for(var i=0; i<fields.length; i++) {
			var field = fields[i];
			//Log.info("field:", field);
			field_defs.push(field.createSqlScript(options));
			var sql_type = field.createTypesSqlScript(options);
			if(sql_type)
				sql_types.push(sql_type);
		}
		for(var i=0; i<indexes.length; i++) {
			var constr = indexes[i].createSqlConstraintScript(options);
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
				ret.push('CREATE INDEX ' + index.indexName(i, name) + ' ON ' + options.schemaName + '.' + root.name + ' ' + index_def);
		}
		for(var i=0; i<fields.length; i++) {
			var fld = fields[i];
			if(fld.comment) {
				ret.push('COMMENT ON COLUMN ' + options.schemaName + '.' + root.name + '.' + fld.name + " IS '" + fld.comment + "'");
			}
		}
		if(root.comment) {
			ret.push('COMMENT ON TABLE ' + options.schemaName + '.' + root.name + " IS '" + root.comment + "'");
		}
		return ret;
	}
}
