import "private"

FieldType {
	property bool primaryKey: false
	property int length: -1 // number of bits
	property bool unsigned: false // AFAIK postgresql doesn't support unsigned

	function createSqlScript(options)
	{
		var def = (length > 32)? 'bigint': 'integer';
		if(primaryKey)
			def += " PRIMARY KEY";
		return def;
	}

	function metaTypeNameFn()
	{
		return "int";
	}
}
