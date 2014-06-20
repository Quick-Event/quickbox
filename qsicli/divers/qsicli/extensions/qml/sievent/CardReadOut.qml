import QtQml 2.0
import qf.core 1.0

QtObject {

    function onSqlConnected(connected)
	{
        Log.debug('SIEvent qsicli extension SQL server connected: ', connected);
	}

	function onCardReadOut(card_id)
	{
        Log.debug('SIEvent qsicli extension onCardReadOut(): card ID:', card_id);
	}

	Component.onCompleted: {
        /*
        console.debug("APP", TheApp);
        var app = TheApp;
        for(var k in app) {
            console.debug("key:",  k);
        }
        */
        TheApp.sqlConnected.connect(onSqlConnected);
        TheApp.cardReadOut.connect(onCardReadOut);
        Log.info("SiEvent CardReadOutExtension", "installed");
	}
}
