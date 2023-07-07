import "private"

FieldType
{
	// If character varying is used without length specifier, the type accepts strings of any size. The latter is a PostgreSQL extension.
	function createSqlScript(options)
	{
		var def = 'time';
		return def;
	}

	function metaTypeNameFn()
	{
		return "QTime";
	}
}
