.pragma library
//import "qrc:/qf/core/qml/js/stringext.js" as StringExt

function Options(opts)
{
	this.options = opts;
}

Options.prototype.fullTableName = function(table_name)
{
	var full_table_name = table_name
	if(this.options.driverName.endsWith("PSQL")) {
		full_table_name = this.options.schemaName + '.' + full_table_name;
	}
	return full_table_name;
}

Options.prototype.isEnumTypeSupported = function()
{
	var ret = this.options.driverName.endsWith("PSQL");
	return ret;
}

Options.prototype.isTableCommentsSupported = function()
{
	var ret = this.options.driverName.endsWith("PSQL");
	return ret;
}

Options.prototype.isString = function(o)
{
    return typeof o == "string" || (typeof o == "object" && o.constructor === String);
}

Options.prototype.quoteSqlValue = function(val)
{
	var ret;
	if(typeof val === 'undefined')
		ret = 'NULL';
	else if(val === null)
		ret = 'NULL';
	else if(this.isString(val))
		ret = "'" + val + "'";
	else if(isNaN(val)) {
		if(this.options.driverName.endsWith("PSQL"))
			ret = 'DEFAULT';
		else
			ret = 'NULL'
	}
	else
		ret = val + "";
	return ret;
}