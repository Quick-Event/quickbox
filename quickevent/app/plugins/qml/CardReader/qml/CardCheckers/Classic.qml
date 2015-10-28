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
		//Log.info("course:", JSON.stringify(course, null, 2));
		
		var checked_card = {courseId: course.id, runId: run_id, punches: []};
		if(!course)
			return checked_card;

		var read_punches = read_card.punches;
		var checked_punches = checked_card.punches;
		for(var k=0; k<read_punches.length; k++) {
			var read_punch = read_punches[k];
			checked_punches.push({code: read_punch.code});
		}

		var course_codes = course.codes;
		var error = false;
		var check_ix = 0;
		if(course_codes) {
			for(var j=0; j<course_codes.length; j++) { //scan course codes
				var course_code_record = course_codes[j];

				for(var k=check_ix; k<checked_punches.length; k++) { //scan card
					//var read_punch = read_punches[k];
					var checked_punch = checked_punches[k];
					if(checked_punch.code === course_code_record.code /*|| checked_punch.code === course_code_record.altcode*/) {
						checked_punch.position = course_code_record.position;
						check_ix = k + 1;
						break;
					}
				}
				if(k >= checked_punches.length) {// course code not found in read_card punches
					//var nocheck = course_code_record.outoforder; for postgres, Query.values() should return lower case keys
					var nocheck = course_code_record.outoforder;
					if(!nocheck)
						error = true;
				}
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
		for(var k=0; k<read_punches.length; k++) { //compute lap times
			var read_punch = read_punches[k];
			var checked_punch = checked_punches[k];
			checked_punch.stpTimeMs = root.msecIntervalAM(checked_card.stageStartTimeMs + checked_card.startTimeMs, read_punch.time * 1000 + read_punch.msec);
			checked_punch.lapTimeMs = 0;
			if(checked_punch.position > prev_position) {  // positions are starting with 1, like 1,2,3,4,5
				if(checked_punch.position - 1 == prev_position) {  
					checked_punch.lapTimeMs = checked_punch.stpTimeMs - prev_position_stp;
				}
				prev_position = checked_punch.position;
				prev_position_stp = checked_punch.stpTimeMs;
			}
		}
		checked_card.finishStpTimeMs = root.msecIntervalAM(checked_card.startTimeMs, checked_card.finishTimeMs);
		checked_card.finishLapTimeMs = checked_card.finishStpTimeMs - prev_position_stp;
		console.debug("check result:", JSON.stringify(checked_card, null, 2));
		return checked_card;
	}
}
