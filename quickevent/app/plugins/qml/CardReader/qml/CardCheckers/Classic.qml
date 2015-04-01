import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import CardReader 1.0

CardChecker 
{
	id: root
	caption: "Classic"
	
	function checkCard(card, run_id)
	{
		Log.info("checking card:", JSON.stringify(card, null, 2));

		var course = root.courseForRunId(run_id);
		Log.info("course:", JSON.stringify(course, null, 2));
		card.courseId = course.id;

		var course_codes = course.codes;
		var punches = card.punchList;
		var error = false;
		var check_ix = 0;
		for(var j=0; j<course_codes.length; j++) { //scan course codes
			// pokud flags obsahuji 'NOCHECK' skoc na dalsi kod
			var course_code_record = course_codes[j];

			var code = course_code_record.code;
			for(var k=check_ix; k<punches.length; k++) { //scan card
				var punch = punches[k];
				if(punch.code == code) {  
					punch.position = course_code_record.position;
					check_ix = k + 1;
					break;
				}
			}
			if(k >= punches.length) {// course code not found in card punches
				var nocheck = course_code_record.outoforder;
				if(!nocheck) 
					error = true;
			}
		}
		if(card.finishTime == 0xEEEE) 
			error = true;
		card.isOk = !error

		//........... normalize times .....................
		card.startTimeMs = 0;
		if(card.startTime == 0xEEEE)        //take start record from start list
			card.startTimeMs = root.stageStartSec() + root.startTimeSec(run_id);
		else 
			card.startTimeMs = card.startTime;
		card.startTimeMs = root.toAMms(card.startTimeMs * 1000);

		card.lapTimeMs = 0;
		if(card.finishTime != 0xEEEE) 
			card.lapTimeMs = root.toAMms(1000 * (card.finishTime - card.startTime) + card.finishTimeMs);

		var prev_position = 0;
		var prev_position_stp = 0;
		for(var k=0; k<punches.length; k++) { //compute lap times
			var punch = punches[k];
			punch.stpTimeMs = root.toAMms(punch.timeMs - card.startTimeMs);
			punch.lapTimeMs = 0;
			if(punch.position > prev_position) {  
				if(punch.position - 1 == prev_position) {  
					punch.lapTimeMs = root.toAMms(punch.stpTimeMs - prev_position_stp);
				}
				prev_position = punch.position;
				prev_position_stp = punch.stpTimeMs;
			}
		}
		Log.info("check result:", JSON.stringify(card, null, 2));
		return card;
	}
}