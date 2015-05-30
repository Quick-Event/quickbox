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
			course = root.courseForRunId(run_id);
		//Log.info("course:", JSON.stringify(course, null, 2));
		
		var checked_card = {courseId: course.id, runId: run_id, punches: []};
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

				var code = course_code_record.code;
				for(var k=check_ix; k<checked_punches.length; k++) { //scan card
					//var read_punch = read_punches[k];
					var checked_punch = checked_punches[k];
					if(checked_punch.code === code) {
						checked_punch.position = course_code_record.position;
						check_ix = k + 1;
						break;
					}
				}
				if(k >= checked_punches.length) {// course code not found in read_card punches
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
		checked_card.checkTimeMs = 0;
		if(read_card.checkTime !== 0xEEEE) {
			checked_card.checkTimeMs = read_card.checkTime * 1000;
		}
		var start_time_sec = 0;
		if(read_card.startTime === 0xEEEE) {        //take start record from start list
			if(run_id > 0)
				start_time_sec = root.startTimeSec(run_id);
			if(start_time_sec === 0) {
				// take start from check, for testing only
				start_time_sec = (((read_card.checkTime / 60) >> 0) + 1) * 60;
				console.warn("Taking start time from check for debugging purposes only, start time;", start_time_sec / 60, read_card.checkTime)
			}
			else {
				start_time_sec = root.stageStartSec() + start_time_sec;
			}
		}
		else {
			start_time_sec = read_card.startTime;
		}
		
		// set start time to be AM even if it is night race, SI cards have 12 hrs wrap-around
		checked_card.startTimeMs = root.toAMms(start_time_sec * 1000);

		checked_card.finishTimeMs = 0;
		//checked_card.lapTimeMs = 0;
		if(read_card.finishTime !== 0xEEEE) {
			//checked_card.finishTimeMs = 1000 * read_card.finishTime;
			checked_card.finishTimeMs = root.fixTimeWrap(checked_card.startTimeMs, 1000 * read_card.finishTime + read_card.finishTimeMs);
		}

		var prev_position = 0;
		var prev_position_stp = 0;
		for(var k=0; k<read_punches.length; k++) { //compute lap times
			var read_punch = read_punches[k];
			var checked_punch = checked_punches[k];
			checked_punch.stpTimeMs = root.toAMms(read_punch.time * 1000 + read_punch.msec - checked_card.startTimeMs);
			checked_punch.lapTimeMs = 0;
			if(checked_punch.position > prev_position) {  // positions are starting with 1, like 1,2,3,4,5
				if(checked_punch.position - 1 == prev_position) {  
					checked_punch.lapTimeMs = checked_punch.stpTimeMs - prev_position_stp;
				}
				prev_position = checked_punch.position;
				prev_position_stp = checked_punch.stpTimeMs;
			}
		}
		checked_card.finishStpTimeMs = checked_card.finishTimeMs - checked_card.startTimeMs;
		checked_card.finishLapTimeMs = checked_card.finishStpTimeMs - prev_position_stp;
		console.debug("check result:", JSON.stringify(checked_card, null, 2));
		return checked_card;
	}
}
