Int
{
	function createSqlScript(options)
	{
		var def = (length > 32)? 'bigserial': 'serial';
		return def;
	}
}
