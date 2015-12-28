.pragma library

function msecSinceMidnight(d)
{
	var e = new Date(d);
	return d - e.setHours(0,0,0,0);
}

function dateToISOString(d)
{
	return d.toISOString().slice(0, 10);
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

