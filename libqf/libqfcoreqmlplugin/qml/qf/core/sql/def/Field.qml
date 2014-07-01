import QtQml 2.0
import qf.core 1.0
import "private"

QtObject {
	property string name
	property FieldType type
	property int length: -1
	property var defaultValue
	property bool notNull: false
	property string comment

	function createSqlScript(options)
	{
		var def = '\t' + name + ' ' + type.createSqlScript(options);
		if(notNull)
			def += ' NOT NULL';
		if(defaultValue) {
			def += ' DEFAULT ';
			if(typeof defaultValue === 'number')
				def += "'" + defaultValue + "'";
			else
				def += defaultValue;
		}
		return def;
	}
}
