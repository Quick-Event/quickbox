.pragma library

function msecToString(msec, sec_sep, msec_sep)
{
	if(msec < 0) {
		console.warn("negative numbers conversion is not implemented properly");
		return "" + (msec / 1000);
	}

	if(!sec_sep)
		sec_sep = "."
	var ms = msec % 1000;
	var sec = ((msec / 1000) >> 0) % 60;
	var min = (msec / (1000 * 60)) >> 0;
	var ret = min + sec_sep;
	if(sec < 10)
		ret += '0';
	ret += sec;
	if(msec_sep) {
		ret += msec_sep;
		if(ms < 100)
			ret += '0';
		if(ms < 10)
			ret += '0';
		ret += ms;
	}
	return ret;
}

function msecToHMS(msec)
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

