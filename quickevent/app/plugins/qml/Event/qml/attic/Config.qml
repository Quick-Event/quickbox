import QtQml 2.0
import qf.core 1.0
//import "qrc:/qf/core/qml/js/stringext.js" as StringExt

QtObject {
	id: root
	property QfObject internal: QfObject {
		property var data: new Object();
		SqlConnection {
			id: db
		}
	}

	function getValue(key, default_value)
	{
		var ret = default_value;
		if(internal.data.hasOwnProperty(key)) {
			ret = internal.data[key];
		}
		return ret;
	}

	function setValue(key, value)
	{
		internal.data[key] = value;
	}

	function reload()
	{
		var q = db.createQuery();
		var qb = q.createBuilder();
		qb.select("ckey, cvalue, ctype").from("config");
		var d = {}
		if(q.exec(qb)) while(q.next()) {
			var key = q.value(0);
			var value = q.value(1);
			var type = q.value(2);
			d[key] = Sql.retypeStringValue(value, type);
		}
		internal.data = d;
	}

	function save()
	{
		var q = db.createQuery();
		var d = internal.data;
		for (var key in d) {
	 		if(d.hasOwnProperty(key)) {
	 			var val = d[key];
				q.prepare('UPDATE config SET cvalue=:val WHERE ckey=:key');
				q.bindValue(':key', key); 
				q.bindValue(':val', val); 
				q.exec();
				if(q.numRowsAffected() < 1) {
		 			var type = Sql.typeNameForValue(val);
					q.prepare('INSERT INTO config (ckey, cvalue, ctype) VALUES (:key, :val, :type)');
					q.bindValue(':key', key); 
					q.bindValue(':type', type); 
					q.bindValue(':val', val); 
					q.exec();
				}
			}	 
		}
	}
}