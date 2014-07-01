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

	function indexName(cnt)
	{
		var ret = name;
		if(!ret) {
			if(primary) {
				ret = 'ix_pkey';
			}
			else if(unique) {
				ret = 'ix_unque' + cnt;
			}
			else {
				ret = 'ix_' + cnt;
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
				ret += '\tCONSTRAINT ' + indexName() + ' PRIMARY KEY (' + fields.join(', ') + ')';
			}
			else if(unique) {
				constr_cnt++;
				ret += '\tCONSTRAINT ' + indexName(constr_cnt) + ' UNIQUE (' + fields.join(', ') + ')';
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
