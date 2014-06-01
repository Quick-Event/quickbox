/// SIEvent qsicli extension
(function() {

var SuperExtension = qf.require('Extension');
function Extension()
{
	qf.log.info('SIEvent qsicli constructor');
	Extension.prototype.__super.call(this);
}
qf.extend(Extension, SuperExtension);

Extension.prototype.eventContext = function()
{
	//debugger
	if(this._eventContext) return this._eventContext;
	var ctx = {};
	var q = new (qf.require("qf.sql.Query"));
	var qs = "SELECT value FROM config WHERE ckey='currEtapNo'";
	var rs = q.exec(qs);
	if(rs && rs.rowCount()) {
		var row = rs.row(0);
		var curr_etapp_no = row.value(0);
		ctx.etap = curr_etapp_no;
		qs = "SELECT start00 FROM etaps WHERE etap=" + curr_etapp_no;
		rs = q.exec(qs);
		if(rs && rs.rowCount()) {
			var row = rs.row(0);
			var start00 = row.value('start00').split(':');
			ctx.start00 = start00[0] * 3600 + start00[1] * 60;
		}
		else { throw new Error(qf.tr("ERROR eventContext()")); }
	}
	else { throw new Error(qf.tr("ERROR eventContext()")); }
	qf.log.info('created event context', qf.toJson(ctx));
	this._eventContext = ctx;
	return this._eventContext;
}
//Extension.prototype.__defineGetter__("eventContext", Extension.prototype.getEventContext);
//Extension.prototype.__defineSetter__("eventContext", function(h) {return this.data.heights = h});

Extension.prototype.onSQLConnect = function(database)
{
	qf.log.info('SIEvent qsicli extension onSqlConnect() called');
	this._eventContext = null;
}

function toAM(secs) 
{
	while(secs > 12*60*60) secs -= 12*60*60;
	while(secs < 0) secs += 12*60*60;
	return secs;
}

Extension.prototype.onCardReadOut = function(card)
{
	qf.log.info('SIEvent qsicli extension onCardReadOut()', qf.toJson(card));
	var dm = Extension.prototype.__super.prototype.onCardReadOut.call(this, card);
	var ret = {};
	if(!driver.isSqlConnectionOpen()) return ret;
 
	var ctx = this.eventContext();
	var q = new (qf.require("qf.sql.Query"));
	var qs = "SELECT runners.ID AS runners_id, classname,name,reg,SItag, laps.ID AS laps_id, laps.start AS laps_start,"
              + " IDrelay, leg"
              + " FROM runners LEFT JOIN laps ON runners.ID=laps.IDrunner "
              + " WHERE laps.IDSI=" + card.cardNumberFull
              + "  AND laps.etap=" + ctx.etap;
	qf.log.info(qs);
	var rs = q.exec(qs);
	var pocet_zavodniku = rs.rowCount();
	if(pocet_zavodniku < 1) {
		ret.retType = "error";
		ret.error = "SI_NOT_FOUND";
		ret.message = qf.tr("SI ${SI} NOT FOUND").replace("${SI}", card.cardNumber);
		return ret;
		//throw new Error(qf.tr("SI ${SI} NOT FOUND").replace("${SI}", card.cardNumber));
	} 
	else if(pocet_zavodniku > 1) {
		ret.retType = "error";
		ret.error = "SI_NOT_UNIQUE";
		ret.message = qf.tr("SI ${SI} IS NOT UNIQUE").replace("${SI}", card.cardNumber);
		return ret;
		//throw new Error(qf.tr("SI ${SI} IS NOT UNIQUE").replace("${SI}", card.cardNumber));
	}
	else {
		var row = rs.row(0);
        var name = row.value("name");
        var reg = row.value("reg");
        var classname = row.value("classname");
        var id_runner = row.value("runners_id");
		
		ret.retType = "info";
		ret.message = card.cardNumber;
		ret.message += " " + name;
		ret.message += " " + classname;

        // pro stafety
        var id_relay = row.value("IDrelay");
        var leg = row.value("leg");
        var id_course = id_relay*10 + leg;

        var sitag = row.value("SItag");
		
        var start_time = row.value("laps_start");
        //start += startErrorShiftSec; // koriguj pripadnou chybu startera

		var relays = false;
		if(!relays) {
			rs = q.exec("SELECT * FROM classcodes WHERE classname='" + classname + "' "
						+ "AND etap=" + ctx.etap + " ORDER BY ctrlno");
		}
		else {
			rs = q.exec("SELECT * FROM classcodes WHERE IDcourse=" + idcourse +
						" AND etap=" + ctx.etap + " ORDER BY ctrlno");
		}
	//....................... kontrola razeni ...................
		var error = false;
		var check_str = "OK";
		var checkix = 0;
		var punches = card.punchList;
		var checks = [];
		for(var k=0; k<punches.length; k++) checks[k] = '!';
		for(var j=0; j<rs.rowCount(); j++) { //projed kontrolni razeni
			// pokud flags obsahuji 'NOCHECK' skoc na dalsi kod
			var row = rs.row(j);
			var flags = row.value("flags");
			var nocheck = (flags.indexOf("NOCHECK") >= 0);
			if(nocheck) continue;

			var code = row.value("code");
			for(var k=checkix; k<punches.length; k++) { //projed kartu
				var punch = punches[k];
				if(punch.code == code) {  //nasel kod v karte
					checks[k] = '*';      //kod OK
					checkix = k+1;
					break;
				}
			}
			if(k >= punches.length) {// karta je u konce nema cenu hledat dal
				error = true;
				//break; // neukoncuj prohledavani, aby slo najit chybejici kontroly
				// siprint uz nepouziva kontrolu razeni z sicli, takze se prohledavani muze klido ukoncit
			}
		}
		if(card.finishTime == 0xEEEE) error = true;
		if(error) check_str = "ERROR";
		//else card->punchingCheck = SICard::checkOk;

		//........... vypocitej cas na trati .....................
		if(card.startTime == 0xEEEE) {       //nerazil start vem ho ze startovky
			start_time = ctx.start00 + start_time;      //odecti startovni casy
		} else {
			start_time = card.startTime;
		}
		var lap_time = 0xEEEE;
		if(card.finishTime != 0xEEEE) lap_time = toAM(card.finishTime - start_time);

		//..........  zapise cas to laps ............................
		//FString checkstr = (card->punchingCheck == SICard::checkOk)? "OK": "ERROR";
		var status_str = (check_str == "OK")? "OK": "DISK";
		qs = "UPDATE laps SET"
				+ "  laptime=" + lap_time + ","
				+ "  status='" + status_str + "',"
				+ "  cardcheck='" + check_str + "'"
				+ " WHERE IDrunner=" + id_runner
				+ "  AND etap=" + ctx.etap;
		q.exec(qs);
		//ret.message += qs;
		ret.message += " " + Math.floor(lap_time / 60) + "." + ("00" + (lap_time % 60)).slice(-2);
		ret.message += " " + check_str;
/*
        // pokud to jsou stafety a vse je v poradku
        if(relays) {
            int legcnt = Config.KeyAsInt("RelaysLegCnt", 3);
            if(laptime != 0xEEEE) {
                // vypln start dalsimu useku
                FTime nextlegstart = laptime + start;
                int nextleg = leg+1;
                fs = "SELECT runners.ID, laps.IDSI FROM runners LEFT JOIN laps ON runners.ID=laps.IDrunner"
                     " WHERE IDrelay=" + FString(idrelay) +
                     "  AND leg=" + FString(nextleg);
                mt.Select(fs);
                if(mt.RowCnt() != 1) {
                    if(leg<legcnt) {
                        AddMemo("*** ERROR Nepodarilo se najit dalsi usek pro");
                        AddMemo(("    " + classname + "." + FString(leg) + " " + reg + " " + name).Str());
                    }
                }
                else {
                    fs = "UPDATE laps SET"
                         "  start=" + FString(nextlegstart.ValSec()) +
                         " WHERE IDrunner=" + mt.AsFStr("runners.ID") +
                         "  AND etap=" + FString(CurrEtap);
                    mt.CommandThrow(fs);
                }

                // vypocitej kumulativni soucet stafety
                fs = "select sum(laptime) AS sumtime from runners"
                     " left join laps on runners.id=laps.idrunner"
                     " where runners.idrelay=" + FString(idrelay);
                mt.Select(fs);
                int sumtime = mt.AsInt("sumtime");
                mt.Select("SELECT * FROM relays WHERE  ID=" + FString(idrelay));
                if(mt.RowCnt() != 1) {
                    AddMemo("*** ERROR Nepodarilo se najit stafetu: " + AnsiString(idrelay));
                }
                else {
                    fs = mt.AsFStr("status");
                    int tm = sumtime * 1000;
                    mt.EditRow();
                    mt.SetAsInt("timems", tm);
                    mt.SetAsInt("legsFinished", leg);
                    if(fs == "NOT_RUN") {
                        // po dobehu posledniho useku vypln OK
                        if(leg == legcnt) mt.SetAsStr("status", "OK");
                    }
                    mt.PostRow();
                }
            }
            else { // je error
                // DISKni stafetu
                fs = "UPDATE relays SET status='DISK' WHERE ID=" + FString(idrelay);
                mt.CommandThrow(fs);
            }
        } // if(relays)
*/
        //.........  inkrementoje pocitadlo novych zavodniku  ........
		qs = "UPDATE classdefs SET tag=tag+1"
				+ " WHERE classname='" + classname + "'"
				+ "  AND etap=" + ctx.etap;
		q.exec(qs);
/*
        fs = (starttime - start00).toAM().toOBString();
        AddMemo((classname + " " + name + " " + reg +
                " start: " + fs +
                ", cas: " + laptime.toOBString() +
                " " + checkstr + " " + sitag).Str());
*/
    } 
    //.................  insertne do cards ............................
	qs = "INSERT INTO cards ("
			+ "etap,SN,IDSI,stnum,Ctime,Stime,Ftime,punchcnt,punches,punchCheck"
			+ ") VALUES (";
	qs += this.writeCardToString(card, ctx, checks, check_str) + ")";
	q.exec(qs);
	//qf.log.info('SIEvent qsicli extension onCardReadOut() FINISHED returning', qf.toJson(ret));
	return ret;
}

Extension.prototype.writeCardToString = function(card, event_context, checks, check_str)
{
	var ret = "";
	ret += event_context.etap + ",";
	ret += (card.stationCodeNumber? card.stationCodeNumber: 0) + ",";
	ret += card.cardNumberFull + ",";
	ret += (card.startNumber? card.startNumber: 0) + ",";
	ret += card.checkTime + ",";
	ret += card.startTime + ",";
	ret += card.finishTime + ",";
	var punches = card.punchList;
	ret += punches.length + ",'";
	for(var i=0; i<punches.length; i++) {
		if(i > 0) ret += '-';
		ret += ("000" + punches[i].code).slice(-3) + checks[i];
		ret += ("00000" + punches[i].time).slice(-5);
		
	}
	ret += "','";
	//if(card->punchingCheck == SICard::checkOk) fs += "OK";
	//else if(card->punchingCheck == SICard::checkError) fs += "ERROR";
	//else fs += "NOT_FOUND";
	ret += check_str;
	ret += "'";
	return ret;
	
}

return Extension;
})();
