import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import CardReader 1.0

// hard task, should be solved in C++

CardChecker 
{
	id: root
	caption: "Beer race"
	
	function checkCard(card, run_id)
	{
		Log.info("checking card:", JSON.stringify(card, null, 2));
	}
}
