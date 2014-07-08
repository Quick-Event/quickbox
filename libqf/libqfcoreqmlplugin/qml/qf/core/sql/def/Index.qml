import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

QtObject
{
	property string name
    property var fields: []
    property bool primary: false
    property bool unique: false
	property string comment

	function indexName(cnt, table_name)
	{
		var ret = name;
		if(!ret) {
			if(primary) {
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

	function createSqlConstraintScript(options)
	{
		var ret = '';
		var constr_cnt = 0;
		if(fields) {
			if(primary) {
				ret += '\tCONSTRAINT ' + indexName(constr_cnt, options.tableName) + ' PRIMARY KEY (' + fields.join(', ') + ')';
			}
			else if(unique) {
				constr_cnt++;
				ret += '\tCONSTRAINT ' + indexName(constr_cnt, options.tableName) + ' UNIQUE (' + fields.join(', ') + ')';
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
