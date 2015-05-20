import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Recipes 1.0

//import "CardCheckers" as CardCheckers

RecipesPlugin {
	id: root
/*
	property QfObject internals: QfObject
	{
		Component {
			id: cReportViewWidget
			ReportViewWidget {}
		}
	}
	
	cardCheckers: [
		CardCheckers.Classic {},
		CardCheckers.BeerRace {}
	]

	onInstalled:
	{
		//var act_file = FrameWork.menuBar.actionForPath('file');
		//var act_competitors = act_file.addMenuAfter('competitors', qsTr('&Competitors'));
		//var act_print = act_competitors.addMenuInto('print', qsTr('&Print'));
		//act_print.addActionInto(actPrintAll);
		var w = cReceipesPartWidget.createObject(FrameWork);
		w.objectName = "cardReadoutPartWidget";

		root.internals.thisPartWidget = w;
		//console.warn("card readout widget:", w)
		FrameWork.addPartWidget(w, root.manifest.featureId);
	}

	function previewReceipeClassic(card_id)
	{
		Log.info("previewReceipe triggered", card_id);
		var w = cReportViewWidget.createObject(null);
		w.persistentSettingsId = "cardPreview";
		w.windowTitle = qsTr("Receipe");
		w.setReport(root.manifest.homeDir + "/reports/receipeClassic.qml");
		var dt = root.receipeTablesData(card_id);
		w.setData("card", dt.card);
		w.setData("competitor", dt.competitor);
		var dlg = FrameWork.createQmlDialog();
		dlg.setDialogWidget(w);
		dlg.exec();
		dlg.destroy();
	}
*/
}
