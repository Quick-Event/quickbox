import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import CardReader 1.0

import "CardCheckers" as CardCheckers

CardReaderPlugin {
	id: root

	cardCheckers: [
		CardCheckers.Classic {},
		CardCheckers.BeerRace {}
	]
/*
	property QfObject internals: QfObject
	{
		property CardReaderPartWidget thisPartWidget
		Component {
			id: cCardReaderPartWidget
			CardReaderPartWidget {
			}
		}
	}

	onInstalled:
	{
		//var act_file = FrameWork.menuBar.actionForPath('file');
		//var act_competitors = act_file.addMenuAfter('competitors', qsTr('&Competitors'));
		//var act_print = act_competitors.addMenuInto('print', qsTr('&Print'));
		//act_print.addActionInto(actPrintAll);
		var w = cCardReaderPartWidget.createObject(FrameWork);
		w.objectName = "cardReadoutPartWidget";

		root.internals.thisPartWidget = w;
		//console.warn("card readout widget:", w)
		FrameWork.addPartWidget(w, root.manifest.featureId);
	}
*/
}
