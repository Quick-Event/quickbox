import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	property int stageCount: 1

	property string reportTitle: qsTr("Competitors statistics")

	property QfObject internals: QfObject {
		Component {
			id: cCell
			Cell { }
		}
	}

	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon { id: myStyle }
	}
	textStyle: myStyle.textStyleDefault

	width: 210
	height: 297
	hinset: 5
	vinset: 5
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		QuickEventHeaderFooter {
			reportTitle: root.reportTitle
		}
		Frame {
			width: "%"
			height: "%"
			columns: "%,%"
			vinset: 10
			Band {
				id: band
				objectName: "band"
				width: "%"
				height: "%"
				exportAsHtmlTable: true
				Frame {
					Para {
						textStyle: TextStyle {basedOn: "big"}
						text: root.reportTitle;
					}
					Para {
						textStyle: myStyle.textStyleBold
						textFn: function() { var event_cfg = band.data("event"); return event_cfg.name; }
					}
					Para {
						textFn: function() { var event_cfg = band.data("event"); return event_cfg.date; }
					}
					Para {
						textFn: function() { var event_cfg = band.data("event"); return event_cfg.place; }
					}
				}
				Space { height: 5 }
				Frame {
					id: head
					layout: Frame.LayoutHorizontal
					textStyle: myStyle.textStyleBold
					Cell {
						id: cellClassName
						text: qsTr("Class name")
					}
					Component.onCompleted: {
						console.debug("stageCount", root.stageCount)
						for(var i=0; i<root.stageCount; i++) {
							var c = cCell.createObject(null, {"text": "E" + (i+1) + " count maps", "width": 30, "halign": Frame.AlignRight});
							head.addItem(c);
						}
					}
				}

				Detail {
					id: detail
					width: "%"
					layout: Frame.LayoutHorizontal
					//Space { height: 5 }
					Cell {
						width: cellClassName.renderedWidth
						text: detail.data(detail.currentIndex, "classes.name");
					}
					function makeCurrentDataFn(field_name) {
						// make extra capture context for returned closure
						// see: MDN Creating closures in loops: A common mistake
						return function() {
							return detail.data(detail.currentIndex, field_name);
						}
					}
					function makeMissingMapsFn(fld_run_cnt, fld_map_cnt) {
						return function() {
							var run_cnt = detail.data(detail.currentIndex, fld_run_cnt);
							var map_cnt = detail.data(detail.currentIndex, fld_map_cnt);
							return map_cnt - run_cnt;
						}
					}

					Component.onCompleted: {
						//console.warn("=============", root.stageCount)
						for(var i=0; i<root.stageCount; i++) {
							var fld_run_cnt = "e" + (i+1) + "_runsCount";
							var fld_map_cnt = "e" + (i+1) + "_mapCount";
							var run_cnt_fn = detail.makeCurrentDataFn(fld_run_cnt);
							var map_cnt_fn = detail.makeCurrentDataFn(fld_map_cnt);
							var c = cCell.createObject(null, {"width": 10, "halign": Frame.AlignRight});
							c.textFn = run_cnt_fn;
							detail.addItem(c);

							c = cCell.createObject(null, {"width": 10, "halign": Frame.AlignRight});
							c.textFn = map_cnt_fn;
							detail.addItem(c);

							c = cCell.createObject(null, {"width": 10, "halign": Frame.AlignRight});
							c.textFn = detail.makeMissingMapsFn(fld_run_cnt, fld_map_cnt);
							detail.addItem(c);
						}
					}
				}
			}
		}
	}
}


