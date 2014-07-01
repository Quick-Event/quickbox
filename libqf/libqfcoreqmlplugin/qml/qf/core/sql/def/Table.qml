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
		ret.push('-- create table: ' + options.schemaName + '.' + root.name);
		var table_def = 'CREATE TABLE ' + options.schemaName + '.' + root.name + ' (\n';
		var field_defs = [];
		for(var i=0; i<fields.length; i++) {
			field_defs.push(fields[i].createSqlScript(options));
		}
		for(var i=0; i<indexes.length; i++) {
			var constr = indexes[i].createSqlConstraintScript(options);
			if(constr)
				field_defs.push(constr);
		}
		table_def += field_defs.join(',\n');
		table_def += '\n)';
		ret.push(table_def);
		for(var i=0; i<indexes.length; i++) {
			var index = indexes[i];
			var index_def = index.createSqlIndexScript(options);
			if(index_def)
				ret.push('CREATE INDEX ' + index.indexName(i) + ' ON ' + options.schemaName + '.' + root.name + ' ' + index_def);
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
