import "private"

FieldType
{
	property int length: -1
	property string charset /// not used for PostgreSql
	property string collation

	// If character varying is used without length specifier, the type accepts strings of any size. The latter is a PostgreSQL extension.
	function createSqlScript(options)
	{
		var def = 'character varying';
		if(length > 0)
			def += '(' + length + ')';
		var coll = collation;
		if(!coll)
			coll = options.defaultCollation;
		if(coll)
			def = def + ' COLLATE "' + coll + '"';
		return def;
	}
}
