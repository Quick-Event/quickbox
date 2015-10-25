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
		var checked_card = {courseId: course.id, runId: run_id, punches: []};
		if(run_id <= 0)
			return checked_card;
		var course = root.courseCodesForRunId(run_id);
		var course_codes = course.codes;
		var checked_card_punches = checked_card.punches;
		var code_to_pos = {};
		for(var j=0; j<course_codes.length; j++) {
			var c = course_codes[j].code;
			checked_card_punches.push({code: c});
			code_to_pos[c] = j;
		}
		var finish_code = root.finishPunchCode();
		checked_card_punches.push({code: finish_code});
		code_to_pos[finish_code] = j;
		//Log.info("course:", JSON.stringify(course, null, 2));
		/*
		var read_punches = read_card.punches;
		for(var k=0; k<read_punches.length; k++) {
			var read_punch = read_punches[k];
			checked_card_punches.push({code: read_punch.code});
		}
		*/
		var error = false;
		var read_card_punches = read_card.punches;
		var read_card_check_ix = 0;
		for(j=0; j<checked_card_punches.length; j++) { //scan card codes
			var checked_card_punch = checked_card_punches[j];
			//var code = course_code_record.code;
			for(var k=read_card_check_ix; k<read_card_punches.length; k++) { //scan card
				//var read_punch = read_punches[k];
				var read_card_punch = read_card_punches[k];
				// uncomment following when altCode will be present in database
				if(read_card_punch.code === course_codes[j].code /*|| read_card_punch.code === course_codes[j].altcode*/) {
					checked_card_punch = read_card_punch;
					read_card_check_ix = k + 1;
					break;
				}
			}
			if(k >= read_card_punches.length) {// course code not found in read_card punches
				//var nocheck = course_code_record.outoforder; for postgres, Query.values() should return lower case keys
				var nocheck = course_codes[j].outoforder;
				if(!nocheck)
					error = true;
			}
		}

		if(read_card.finishTime === 0xEEEE)
			error = true;
		checked_card.isOk = !error

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
		if(read_card.finishTime !== 0xEEEE) {
			checked_card.finishTimeMs = root.msecIntervalAM(start00sec * 1000, read_card.finishTime * 1000);
		}

		var prev_position = 0;
		var prev_position_stp = 0;
		for(k=0; k<checked_card_punches.length; k++) { //compute lap times
			//var read_punch = read_punches[k];
			checked_card_punch = checked_card_punches[k];
			var stp_time_ms;
			if(k == checked_card_punches.length - 1) {
				stp_time_ms = checked_card.finishTimeMs - checked_card.startTimeMs;
			}
			else {
				stp_time_ms = checked_card_punch.time;
				if(stp_time_ms) {
					stp_time_ms = stp_time_ms * 1000 + checked_card_punch.msec;
					stp_time_ms = root.msecIntervalAM(checked_card.stageStartTimeMs + checked_card.startTimeMs, stp_time_ms);
				}
			}
			if(stp_time_ms) {
				checked_card_punch.stpTimeMs = stp_time_ms;
				checked_card_punch.lapTimeMs = 0;
				if(checked_card_punches[k-1].stpTimeMs) {
					checked_card_punch.lapTimeMs = checked_card_punch.stpTimeMs - checked_card_punches[k-1].stpTimeMs;
				}
			}
		}
		//checked_card.finishStpTimeMs = checked_card_punch.stpTimeMs;
		//checked_card.finishLapTimeMs = checked_card_punch.lapTimeMs;
		console.debug("check result:", JSON.stringify(checked_card, null, 2));
		return checked_card;
	}
}
