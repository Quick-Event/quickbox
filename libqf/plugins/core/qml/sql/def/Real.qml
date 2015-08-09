import "private"

FieldType
{
	property int length: 64 // number of bits, double is default

	function createSqlScript(options)
	{
		var def = (length > 32) 'double precision': 'real';
		return def;
	}

	function variantTypeName()
	{
		return "double";
	}
}
