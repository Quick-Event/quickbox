.pragma library

function msecSinceMidnight(d)
{
	var e = new Date(d);
	return d - e.setHours(0,0,0,0);
}

function dateToISOString(dt)
{
	var y = dt.getFullYear();
	var m = dt.getMonth() + 1;
	if(m < 10)
		m = "0" + m;
	var d = dt.getDate();
	if(d < 10)
		d = "0" + d;
	var ret = y + "-" + m + "-" + d;
	return ret;
}

function dateTimeToUTCISOString(dt)
{
	return dt.toISOString();
}

function dateToUTCISOString(dt)
{
	var s = dateTimeToUTCISOString(dt);
	return s.substr(0, 10);
}

function timeToUTCISOString(dt)
{
	var s = dateTimeToUTCISOString(dt);
	return s.substr(11);
}

function msecToString_hhmmss(msec)
{
	if(msec < 0) {
		console.warn("negative numbers conversion is not implemented properly");
		return "" + (msec / 1000);
	}

	var n = ((msec / 1000) >> 0);
	var sec = n % 60;
	n = (n / 60) >> 0;
	var min = n % 60;
	var hod = (n / 60) >> 0;
	var ret = hod + ":";
	if(min < 10)
		ret += '0';
	ret += min + ":";
	if(sec < 10)
		ret += '0';
	ret += sec;
	return ret;
}

