import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

QtObject
{
	property string name
	property var fields: []
	property bool primary: false
	property bool unique: false
	property ForeignKeyReference references: null
	property string comment

	function indexName(cnt, table_name)
	{
		var ret = name;
		if(!ret) {
			if(references) {
				ret = table_name + '_foreign' + cnt;
			}
			else if(primary) {
				ret = table_name + '_pkey';
			}
			else if(unique) {
				ret = table_name + '_unique' + cnt;
			}
			else {
				ret = table_name + '_ix' + cnt;
			}
		}
		return ret;
	}

	function createSqlConstraintScript(constr_no, options)
	{
		var ret = '';
		var full_table_name = options.tableName;
		if(options.driverName.endsWith("PSQL"))
			full_table_name = options.schemaName + '.' + options.tableName;
		if(fields) {
			if(references) {
				ret += '\tCONSTRAINT ' + indexName(constr_no, options.tableName)
					+ ' FOREIGN KEY ('
					+ fields.join(', ')
					+ ') REFERENCES '
					+ options.schemaName + '.' + references.table + ' ('
					+ references.fields.join(', ')
					+ ')'
					+ ' ON UPDATE ' + references.onUpdate
					+ ' ON DELETE ' + references.onDelete;
			}
			else if(primary) {
				ret += '\tCONSTRAINT ' + indexName(constr_no, options.tableName) + ' PRIMARY KEY (' + fields.join(', ') + ')';
			}
			else if(unique) {
				ret += '\tCONSTRAINT ' + indexName(constr_no, options.tableName) + ' UNIQUE (' + fields.join(', ') + ')';
			}
		}
		return ret;
	}

	function createSqlIndexScript(options)
	{
		var ret = '';
		if(fields) {
			if(!primary && !unique) {
				ret = '(' + fields.join(', ') + ')';
			}
		}
		return ret;
	}
}
