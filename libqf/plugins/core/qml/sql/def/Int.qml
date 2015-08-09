import "private"

FieldType {
	property int length: -1 // number of bits
	property bool unsigned: false // AFAIK postgresql doesn't support unsigned

	function createSqlScript(options)
	{
		var def = (length > 32)? 'bigint': 'integer';
		return def;
	}

	function variantTypeName()
	{
		return "int";
	}
}
