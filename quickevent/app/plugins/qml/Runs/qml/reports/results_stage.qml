import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Results by classes")
	property bool isBreakAfterEachClass: false
	property bool isColumnBreak: false
	property var options

	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			},
			Pen {
				id: pen_black1
				basedOn: "black1"
			}
		]
	}
	textStyle: myStyle.textStyleDefault

	width: (root.options && root.options.isShirinkPageWidthToColumnCount)? 210/2*root.options.columnCount: 210
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
			columns: root.options.columns
			vinset: 10
			Band {
				id: band
				objectName: "band"
				width: "%"
				//height: "%"
				QuickEventReportHeader {
					dataBand: band
					reportTitle: root.reportTitle
				}
				Space { height: 5 }
				Detail {
					id: detail
					objectName: "detail"
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					Break {
						breakType: root.isColumnBreak? Break.Column: Break.Page;
						visible: root.isBreakAfterEachClass;
						skipFirst: true
					}
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							width: "%"
							textFn: detail.dataFn("classes.name");
							textStyle: myStyle.textStyleBold
						}
						Cell {
							textFn: function() { return qsTr("length: ") + detail.rowData("courses.length");}
						}
						Cell {
							textFn: function() { return qsTr("climb: ") + detail.rowData("courses.climb");}
						}
					}
					//expandChildrenFrames: true
					Band {
						id: runnersBand
						objectName: "runnersBand"
						keepFirst: 3
						keepWithPrev: true
						htmlExportAsTable: true
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							function dataFn(field_name) {return function() {return rowData(field_name);}}
							Cell {
								width: 10
								halign: Frame.AlignRight
								textFn: function() {
									return runnersDetail.rowData("pos");
								}
							}
							Cell {
								width: "%"
								textFn: runnersDetail.dataFn("competitorName");
							}
							Para {
								width: 18
								textFn: runnersDetail.dataFn("registration");
							}
							Cell {
								width: 15
								halign: Frame.AlignRight
								textFn: function() { return OGTime.msecToString_mmss(runnersDetail.rowData("timeMs"));}
							}
							Para {
								width: 10
								textFn: function() {
									if(runnersDetail.rowData("disqualified"))
										return qsTr("DISQ");
									if(runnersDetail.rowData("notCompeting"))
										return qsTr("NC");
									return "";
								}
							}
						}
					}
				}
			}
		}
	}
}


