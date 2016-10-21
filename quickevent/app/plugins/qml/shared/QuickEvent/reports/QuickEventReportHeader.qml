import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Frame {
	id: root
	property Band dataBand
	property string reportTitle
	property bool showStageNumber: true
	Para {
		textStyle: TextStyle {basedOn: "big"}
		textFn: function() {
			var ret = root.reportTitle;
			if(root.showStageNumber) {
				var stage_cnt = dataBand.data("event").stageCount
				if(stage_cnt > 1)
					ret = "E" + dataBand.data("stageId") + " " + ret;
			}
			return ret;
		}
	}
	Para {
		textStyle: myStyle.textStyleBold
		textFn: function() { var event_cfg = dataBand.data("event"); return event_cfg.name; }
	}
	Para {
		textFn: function() { return dataBand.data("stageStart"); }
	}
	Para {
		textFn: function() { var event_cfg = dataBand.data("event"); return event_cfg.place; }
	}
}
