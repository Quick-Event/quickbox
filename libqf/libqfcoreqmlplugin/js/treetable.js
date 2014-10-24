.pragma library

function strEndsWith(str, suffix) 
{
    return str.indexOf(suffix, str.length - suffix.length);
}

function sqlEndsWith(full_field_name, short_field_name)
{
	var ix = strEndsWith(full_field_name, short_field_name);
	if(ix == 0) return true;
	else if(ix > 0) {
		/// na posici pred ix musi byt tecka
		return full_field_name.charAt(ix - 1) == ".";
	}
	return false;
}

function Row(flds)
{
	if(arguments.length > 0) {
		this.fields = flds;
		this.values = new Array(flds.length);
	}
	else {
		this.fields = new Array();
		this.values = new Array();
	}
	this.keyvals = null; //new Object();
	this.tables = new Object()
}

Row.prototype.value = function(ix)
{
	var ret = undefined;
	if(typeof(ix) == 'number') {
		if(ix < this.values.length) ret = this.values[ix];
	}
	else {
		var fld_name = ix.toString();
		for(var i=0; i<this.fields.length; i++) {
			var fld = this.fields[i];
			if(sqlEndsWith(fld.name, fld_name)) {
				ret = this.values[i];
				break;
			}
		}
	}
	return ret;
}

Row.prototype.setValue = function(ix, val)
{
//driver.logInfo("setValue index: " + ix + " value: " + val);
	if(typeof(ix) == 'number') {
	//driver.logInfo("numerical index, values length: " + this.values.length);
	//driver.logInfo("old value: " + this.values[ix]);
		if(ix < this.values.length) this.values[ix] = val;
	//driver.logInfo("new values length: " + this.values.length);
	}
	else {
		var fld_name = ix.toString();
		for(var i=0; i<this.fields.length; i++) {
			var fld = this.fields[i];
			if(sqlEndsWith(fld.name, fld_name)) {
				this.values[i] = val;
				break;
			}
		}
	}
}

Row.prototype.fieldCount = function()
{
	return this.fields.length;
}

Row.prototype.addTable = function(tbl, name)
{
	return this.tables[name] = tbl;
}

function Table(flds)
{
	//driver.logInfo("flds: " + qf.toJson(flds));
	//driver.logInfo("arguments.length: " + arguments.length);
	if(arguments.length > 0) {
		this.fields = flds;
	}
	else {
		this.fields = new Array();
	}
	this.rows = new Array();
	this.meta = null; //new Object();
	this.keyvals = null; //new Object();
}

//Table.prototype.__qfTypeName = 'qf.sql.Table';
Table.prototype.Row = Row;

Table.prototype.emptyRow = new Row();

Table.prototype.row = function(ix)
{
	if(ix >= 0 && ix < this.rows.length) {
		return this.rows[ix];
	}
	return this.emptyRow;
}

Table.prototype.appendRow = function(r)
{
//driver.logInfo("appendRow before row count: " + this.rows.length);
//driver.logInfo("row: " + Qf.toJson(r));
	this.rows.push(r);
//driver.logInfo("appendRow after row count: " + this.rows.length);
}

Table.prototype.fieldCount = function()
{
	return this.fields.length;
}

Table.prototype.rowCount = function()
{
	return this.rows.length;
}

Table.prototype.setContent = function(obj)
{
	this.rows = null;
	this.tables = null;
	if(obj) {
		this.meta = obj.meta;
		this.fields = obj.fields;
		this.rows = obj.rows;
		this.keyvals = obj.keyvals;
		if(obj.tables) {
			this.tables = new Array();
			for(var i=0; i<obj.tables.length; i++) {
				var src_t = obj.tables[i];
				var t = new Table(src_t.fields); // TODO make deep copy of fields
				this.tables.push(t);
			}
		}
	}
	else {
		this.meta = null;
		this.fields = null;
		this.keyvals = null;
	}
}

