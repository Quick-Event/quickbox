import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import CardReader 1.0

CardChecker 
{
	id: root
	caption: "Classic"
	
	function checkCard(read_card)
	{
		console.debug("read card:", JSON.stringify(read_card, null, 2));

		var run_id = read_card.runId;
		var course = {};
		if(run_id > 0)
			course = root.courseCodesForRunId(run_id);
		//Log.info("course:", JSON.stringify(read_card, null, 2));
		
		var checked_card = {courseId: course.id, runId: run_id, punches: []};
		if(!course.id)
			return checked_card;

		var error_mis_punch = false;
		var checked_punches = checked_card.punches;
		var course_codes = course.codes;
		var read_punches = read_card.punches;

		//........... normalize times .....................
		// checked card times are in msec relative to run start time
		// startTime, checkTime and finishTime in in msec relative to event start time 00
		var start00sec = root.stageStartSec();
		checked_card.stageStartTimeMs = start00sec * 1000;
		checked_card.checkTimeMs = null;
		checked_card.startTimeMs = null;
		if(read_card.checkTime !== 0xEEEE) {
			checked_card.checkTimeMs = root.msecIntervalAM(start00sec * 1000, read_card.checkTime * 1000);
		}
		//var start_time_sec = null;
		if(read_card.startTime === 0xEEEE) {        //take start record from start list
			if(run_id > 0) {
				checked_card.startTimeMs = root.startTimeSec(run_id) * 1000;
				//console.warn(start_time_sec);
			}
			var is_debug = false;
			if(is_debug && !start_time_sec && checked_card.checkTimeMs > 0) {
				// take start from check if zero, for testing only
				checked_card.startTimeMs = (((checked_card.checkTimeMs / 60000) >> 0) + 1) * 60000;
				console.warn("Taking start time from check for debugging purposes only, start time;", checked_card.startTimeMs / 60000);
			}
		}
		else {
			checked_card.startTimeMs = root.msecIntervalAM(start00sec * 1000, read_card.startTime * 1000);
		}
		
		checked_card.finishTimeMs = null;
		if(read_card.finishTime === 0xEEEE) {
			error_mis_punch = true;
		}
		else {
			checked_card.finishTimeMs = root.msecIntervalAM(start00sec * 1000, read_card.finishTime * 1000);
			if(read_card.finishTimeMs)
				checked_card.finishTimeMs += read_card.finishTimeMs;
		}

		var read_punch_check_ix = 0;
		for(var j=0; j<course_codes.length; j++) {
			var course_code = course_codes[j];
			var checked_punch = {code: course_code.code};
			checked_punches.push(checked_punch);
			for(var k=read_punch_check_ix; k<read_punches.length; k++) { //scan card
				var read_punch = read_punches[k];
				if(read_punch.code === course_code.code || read_punch.code === course_code.altcode) {
					var read_punch_time_ms = read_punch.time * 1000;
					if(read_punch.msec)
						read_punch_time_ms += read_punch.msec;
					checked_punch.stpTimeMs = root.msecIntervalAM(checked_card.stageStartTimeMs + checked_card.startTimeMs, read_punch_time_ms);
					break;
				}
			}
			if(k === read_punches.length) {
				// code not found
				if(!course_code.outoforder) // for postgres, Query.values() should return lower case keys
					error_mis_punch = true;
			}
			else {
				read_punch_check_ix = k + 1;
			}
		}
		checked_card.isMisPunch = error_mis_punch;

		var finish_punch = {
			code: root.FINISH_PUNCH_CODE,
			stpTimeMs: root.msecIntervalAM(checked_card.startTimeMs, checked_card.finishTimeMs)
		}
		checked_punches.push(finish_punch);

		var prev_stp_time_ms = 0;
		for(k=0; k<checked_punches.length; k++) {
			checked_punch = checked_punches[k];
			if(checked_punch.stpTimeMs) {
				if(prev_stp_time_ms !== null)
					checked_punch.lapTimeMs = checked_punch.stpTimeMs - prev_stp_time_ms;
				prev_stp_time_ms = checked_punch.stpTimeMs;
			}
			else {
				prev_stp_time_ms = null;
			}
		}

		console.debug("check result:", JSON.stringify(checked_card, null, 2));
		return checked_card;
	}
}
