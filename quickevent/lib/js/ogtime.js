.pragma library

function msecToString_mmss(msec, sec_sep, msec_sep)
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

function fixTimeWrapAM(time1_msec, time2_msec)
{
	var hr12ms = 12 * 60 * 60 * 1000;
	while(time2_msec < time1_msec)
		time2_msec += hr12ms;
	while(time1_msec <= time2_msec - hr12ms)
		time2_msec -= hr12ms;
	return time2_msec;
}

function msecIntervalAM(time1_msec, time2_msec)
{
	return fixTimeWrapAM(time1_msec, time2_msec) - time1_msec;
}

var UNREAL_TIME_MS = 9999 * 60 * 1000;
var NOT_FINISH_TIME_MSEC = UNREAL_TIME_MS
var NOT_COMPETITING_TIME_MSEC = NOT_FINISH_TIME_MSEC - 1;
var DISQ_TIME_MSEC = NOT_COMPETITING_TIME_MSEC - 1;

