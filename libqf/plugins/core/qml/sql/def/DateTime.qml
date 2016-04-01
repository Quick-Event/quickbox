import "private"

FieldType
{
	function createSqlScript(options)
	{
		var def = 'timestamp';
		if(options.driverName.endsWith("PSQL")) {
			def += " with time zone";
		}
		return def;
	}

	function metaTypeNameFn()
	{
		return "QDateTime";
	}
}
