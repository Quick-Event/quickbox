.pragma library

function strEndsWith(str, suffix)
{
	if(str && str.indexOf)
		return str.indexOf(suffix, str.length - suffix.length);
	return 0;
}

function sqlEndsWith(full_field_name, short_field_name)
{
	var ix = strEndsWith(full_field_name, short_field_name);
	if(ix === 0)
		return true;
	else if(ix > 0) {
		/// na posici pred ix musi byt tecka
		return full_field_name.charAt(ix - 1) === ".";
	}
	return false;
}

function Table(data)
{
	this._data = data;
}

Table.prototype.rowCount = function()
{
	var ret = 0;
	if(this._data && this._data.rows && this._data.rows instanceof Array)
		ret = this._data.rows.length;
	return ret;
}

Table.prototype.columnCount = function()
{
	var ret = 0;
	if(this._data && this._data.fields && this._data.fields instanceof Array)
		ret = this._data.fields.length;
	return ret;
}

Table.prototype.column = function(ix)
{
	var ret = null;
	if(this._data && this._data.fields && this._data.fields instanceof Array)
		if(ix >= 0 && ix < this._data.fields.length)
			ret = this._data.fields[ix];
	return ret;
}

Table.prototype.addColumn = function(col)
{
	if(this._data) {
		if(!this._data.fields || !(this._data.fields instanceof Array)) {
			this._data.fields = [];
		}
		this._data.fields.push(col)
	}
}

Table.prototype.name = function()
{
	var ret = null;
	if(this._data && this._data.meta) {
		ret = this._data.meta.name;
	}
	return ret;
}

Table.prototype.setName = function(nm)
{
	if(this._data) {
		if(!this._data.meta || !(this._data.meta instanceof Object)) {
			this._data.meta = new Object();
		}
		this._data.meta.name = nm;
	}
}

Table.prototype.value = function(row_ix, col_ix)
{
	var ret = undefined;
	if(typeof(row_ix) == 'number') {
		if(this._data.rows && this._data.rows instanceof Array) {
			if(row_ix >= 0) {
				var row = this._data.rows[row_ix];
				if(row && row instanceof Array) {
					if(typeof(col_ix) == 'number') {
						if(col_ix >= 0)
							ret = row[col_ix];
					}
					else {
						var fld_name = col_ix.toString();
						var fields = this._data.fields;
						if(fields && fields instanceof Array) {
							for(var i=0; i<fields.length; i++) {
								var fld = this.fields[i];
								if(fld && sqlEndsWith(fld.name, fld_name)) {
									ret = row[i];
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		if(this._data.keyvals && this._data.keyvals instanceof Object)
			ret = this._data.keyvals[row_ix];
	}
	return ret;
}

Table.prototype.setValue = function(row_ix, col_ix, val)
{
	var ret = false;
	if(typeof(row_ix) == 'number') {
		if(row_ix >= 0) {
			if(!this._data.rows || !(this._data.rows instanceof Array)) {
				this._data.rows = [];
			}
			var row = this._data.rows[row_ix];
			if(!row || !(row instanceof Array)) {
				this._data.rows[row_ix] = [];
				row = this._data.rows[row_ix];
			}
			if(typeof(col_ix) == 'number') {
				if(col_ix >= 0) {
					row[col_ix];
					ret = true;
				}
			}
			else {
				var fld_name = col_ix.toString();
				var fields = this._data.fields;
				if(fields && fields instanceof Array) {
					for(var i=0; i<fields.length; i++) {
						var fld = fields[i];
						if(fld && sqlEndsWith(fld.name, fld_name)) {
							row[i] = val;
							ret = true;
							break;
						}
					}
				}
			}
		}
	}
	else {
		if(!this._data.keyvals || !(this._data.keyvals instanceof Object)) {
			this._data.keyvals = new Qbject();
		}
		this._data.keyvals[row_ix] = val;
		ret = true;
	}
	return ret;
}

Table.prototype.table = function(row_ix, table_name)
{
	var ret = undefined;
	if(this._data.tables) {
		var table_data = this._data.tables[row_ix];
		if(table_data) {
			ret = new Table(table_data);
		}
	}

	return ret;
}

Table.prototype.setTable = function(row_ix, table)
{
	if(!this._data.tables || !(this._data.tables instanceof Array)) {
		this._data.tables = []
	}
	if(table instanceof Table)
		this._data.tables.push(table.data);
	else if(table instanceof Object)
		this._data.tables.push(table);
}

Table.prototype.data = function()
{
	return this._data;
}

Table.prototype.setData = function(data)
{
	this._data = data;
}

Table.prototype.toString = function()
{
	return JSON.stringify(this._data, null, 2);
}

