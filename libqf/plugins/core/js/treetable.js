.pragma library

function strEndsWith(str, suffix, ignore_case)
{
	if(str && str.indexOf) {
		var s = (ignore_case)? str.toLowerCase(): str;
		var suf = (ignore_case)? suffix.toLowerCase(): suffix;
		return s.indexOf(suf, s.length - suf.length);
	}
	return 0;
}

function sqlEndsWith(full_field_name, short_field_name)
{
	var ix = strEndsWith(full_field_name, short_field_name, true);
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

Table.prototype.isNull = function()
{
	return !(this._data);
}

Table.prototype.rowCount = function()
{
	var ret = 0;
	if(this._data && this._data.rows && this._data.rows instanceof Array)
		ret = this._data.rows.length;
	return ret;
}

Table.prototype.addRow = function(before_ix)
{
	var rows = this._data.rows;
	if(!(rows instanceof Array))
		rows = [];

	if(typeof before_ix === 'undefined')
		before_ix = rows.length;
	//rows.push([]);
	for(var i=rows.length; i>before_ix; i--) {
		rows[i] = rows[i-1];
	}
	rows[before_ix] = [];
	this._data.rows = rows;
}
/*
Table.prototype.appendRow = function()
{
	this.addRow();
}
*/
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

Table.prototype.addColumn = function(col_name, col_type, col_width)
{
	if(this._data) {
		if(!this._data.fields || !(this._data.fields instanceof Array)) {
			this._data.fields = [];
		}
		if(!col_type)
			col_type = "QString";
		this._data.fields.push({name: col_name, type: col_type, width: col_width})
	}
}
/*
Table.prototype.appendRow = function()
{
	if(this._data) {
		if(!(this._data.rows instanceof Array)) {
			this._data.rows = [];
		}
		this._data.fields.push({name: col_name, type: col_type, width: col_width})
	}
	else {
		console.warn("Cannot append row to NULL table!");
	}
}
*/
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
	if(typeof(row_ix) === 'number') {
		// table rows values
		if(this._data.rows && this._data.rows instanceof Array) {
			if(row_ix >= 0) {
				var row_o = this._data.rows[row_ix];
				var row = undefined;
				if(row_o) {
					if(row_o instanceof Array) {
						row = row_o;
					}
					else if(row_o instanceof Object) {
						row = row_o.row // object -> row_line_array + tables_array
					}
				}
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
								var fld = fields[i];
								//console.warn(i, fld_name, fld.name)
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
		// table values
		if(this._data.keyvals && this._data.keyvals instanceof Object)
			ret = this._data.keyvals[row_ix];
	}
	return ret;
}

Table.prototype.setValue = function(row_ix, col_ix, val)
{
	var ret = false;
	if(!this._data) {
		throw "Table is not initialized";
	}
	if(typeof(row_ix) === 'number') {
		// table rows values
		if(row_ix >= 0) {
			if(!this._data.rows || !(this._data.rows instanceof Array)) {
				this._data.rows = [];
			}
			var row_o = this._data.rows[row_ix];
			var row = undefined;
			if(!row_o) {
				row = [];
				this._data.rows[row_ix] = row;
			}
			else if(row_o instanceof Array) {
				row = row_o
			}
			else if(row instanceof Object) {
				row = row_o.row;
				if(!(row_o instanceof Array)) {
					row = [];
					row_o.row = row;
				}
			}
			else {
				throw "Table corrupted";
			}

			if(typeof(col_ix) == 'number') {
				if(col_ix >= 0) {
					row[col_ix] = val;
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
							//console.warn(fld_name, "row:", i, "=", val)
							ret = true;
							break;
						}
					}
				}
				if(!ret)
					console.warn("Cannot find column", fld_name)
			}
		}
	}
	else {
		// table values
		// if row_ix is a string, then col_ix is value, it is wrapper for
		// void TreeTable::setValue(const QString &key_name, const QVariant &val);
		if(!this._data.keyvals || !(this._data.keyvals instanceof Object)) {
			this._data.keyvals = new Object();
		}
		this._data.keyvals[row_ix] = col_ix;
		ret = true;
	}
	return ret;
}

Table.prototype.table = function(row_ix, table_name)
{
	var ret = undefined;
	if(!this._data) {
		throw "Table is not initialized";
	}
	if(typeof(row_ix) == 'number') {
		// table rows values
		if(this._data.rows && this._data.rows instanceof Array) {
			if(row_ix >= 0) {
				var row_o = this._data.rows[row_ix];
				if(row_o && row_o instanceof Object) {
					var tables = row_o.tables;
					if(tables instanceof Array) {
						for(var i=0; i<tables.length; i++) {
							var table_data = tables[i];
							if(table_data && table_data.meta) {
								if(!table_name || table_data.meta.name === table_name) {
									ret = new Table(table_data);
								}
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

Table.prototype.addTable = function(row_ix, table)
{
	if(!this._data || !(this._data instanceof Object)) {
		throw "Invalid table";
	}
	var rows = this._data.rows;
	if(!rows && !(rows instanceof Array)) {
		rows = []
		this._data.rows = rows;
	}
	var row_o = rows[row_ix];
	if(row_o instanceof Array) {
		row_o = {row: row_o, tables: []};
		rows[row_ix] = row_o;
	}
	var tables = row_o.tables;
	if(!(tables instanceof Array)) {
		tables = [];
		row_o.tables = tables;
	}
	if(table instanceof Table)
		tables.push(table.data);
	else if(table instanceof Object)
		tables.push(table);
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

