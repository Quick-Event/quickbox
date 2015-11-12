import "private"

FieldType
{
	function createSqlScript(options)
	{
		var def = 'timestamp';
		return def;
	}

	function metaTypeNameFn()
	{
		return "QDateTime";
	}
}
