import "private"
import "private/libsqldef.js" as SqlDef

FieldType
{
	property string name
	property var keys: []

	function createSqlScript(options)
	{
		if(__isEnumSupported(options))
			var def = options.schemaName + '.' + name;
		else
			var def = 'character varying'
		return def;
	}

	function createSqlType(options)
	{
		var ret = 'CREATE TYPE ' + options.schemaName + '.' + name + ' AS ENUM (' + __quoteArray(keys).join(",") + ')';
		if(!__isEnumSupported(options))
			ret = "-- " + ret;
		return ret;
	}

	function __quoteArray(a)
	{
		var ret = [];
		for(var i=0; i<a.length; i++)
			ret.push("'" + a[i] + "'");
		return ret;
	}

	function __isEnumSupported(options)
	{
		var opts = new SqlDef.Options(options);
		return opts.isEnumTypeSupported();
	}
}
