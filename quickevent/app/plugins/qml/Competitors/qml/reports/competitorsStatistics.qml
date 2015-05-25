import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	property int stage_count: 1

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
						console.warn("##############", root.stage_count)
						for(var i=0; i<root.stage_count; i++) {
							var c = cCell.createObject(null, {"text": "E" + (i+1) + " count", "width": 20});
							head.addItem(c);
						}
					}
				}

				Detail {
					id: detail
					width: "%"
					//keepAll: true
					layout: Frame.LayoutHorizontal
					//function dataFn(field_name) {return function() {return rowData(field_name);}}
					//Space { height: 5 }
					Cell {
						width: cellClassName.renderedWidth
						text: detail.data(detail.currentIndex, "classes.name");
					}
					Cell {
						width: "20"
						halign: Frame.AlignRight
						text: detail.data(detail.currentIndex, "e1_runsCount");
					}
					function currentData(field_name) {
						return function () {
							return detail.data(detail.currentIndex, field_name);
						}
					}

					Component.onCompleted: {
						console.warn("=============", root.stage_count)
						for(var i=0; i<root.stage_count; i++) {
							var c = cCell.createObject(null, {"width": 20});
							var fld_name = "e" + (i+1) + "_runsCount";
							c.textFn = function() { return detail.data(detail.currentIndex, fld_name); }
							detail.addItem(c);
						}
					}
				}
			}
		}
	}
}


