.pragma library
//import "qrc:/qf/core/qml/js/stringext.js" as StringExt

function Options(opts)
{
	this.options = opts;
}

Options.prototype.fullTableName = function()
{
	if(this.options.driverName.endsWith("SQLITE")) {
		var full_table_name = this.options.tableName;
	}
	else {
		var full_table_name = this.options.schemaName + '.' + this.options.tableName;
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