.pragma library

function msecToTimeString(msec)
{
	var ms = msec % 1000;
	var sec = ((msec / 1000) >> 0) % 60;
	var min = (msec / (1000 * 60)) >> 0;
	var hr = (min / 60) >> 0;
	min = min % 60;
	var ret = hr + ':';
	if(min < 10)
		ret += '0';
	ret += min + ':';
	if(sec < 10)
		ret += '0';
	ret += sec;
	return ret;
}

function msecSinceMidnight(d)
{
	var e = new Date(d);
	return d - e.setHours(0,0,0,0);
}

function dateToISOString(d)
{
	return d.toISOString().slice(0, 10);
}
