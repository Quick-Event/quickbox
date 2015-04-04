.pragma library

function msecToString(msec)
{
	var ms = msec % 1000;
	var sec = ((msec / 1000) >> 0) % 60;
	var min = (msec / (1000 * 60)) >> 0;
	var ret = min + ':';
	if(sec < 10)
		ret += '0';
	ret += sec;
	if(ms > 0) {
		ret += '.';
		if(ms < 100)
			ret += '0';
		if(ms < 10)
			ret += '0';
		ret += ms;
	}
	return ret;
}

