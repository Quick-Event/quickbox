import "private"

FieldType
{
	property string name
	property var keys: []

	function createSqlScript(options)
	{
		var def = options.schemaName + '.' + name;
		return def;
	}

	function createSqlType(options)
	{
		return 'CREATE TYPE ' + options.schemaName + '.' + name + ' AS ENUM (' + quoteArray(keys).join(",") + ')';
	}

	function quoteArray(a)
	{
		var ret = [];
		for(var i=0; i<a.length; i++)
			ret.push("'" + a[i] + "'");
		return ret;
	}
}
