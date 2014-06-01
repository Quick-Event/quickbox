/// dummy qsicli script object example
(function() {

//var Super = qf.require('SuperExtension');
function Extension()
{
	//Extension.prototype.__super.call(this);
}
//qf.extend(Extension, SuperExtension);

Extension.prototype.onSQLConnect = function(database)
{
	qf.log.info('Extension dummy prototype onSqlConnect() on database', database);
}

Extension.prototype.onCommConnect = function(device)
{
	qf.log.info('Extension dummy prototype onCommConnect() called on device', device);
}

Extension.prototype.onCardReadOut = function(card)
{
	qf.log.info('Extension dummy prototype onCardReadOut()', qf.toJson(card));
	driver.appendCardLogLine(qf.toJson(card));
}

return Extension;
})();
