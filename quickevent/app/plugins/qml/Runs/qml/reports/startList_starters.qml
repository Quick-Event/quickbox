import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Start list for starters")

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
				QuickEventReportHeader {
					dataBand: band
					reportTitle: root.reportTitle
				}
				Detail {
					id: detail
					objectName: "detail"
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					//Space { height: 5 }
					Frame {
						id: f1
						// hide recent value of start time in object to avoid property binding loop
						// property int prevStartTimeMin: -1 // property binding loop
						property var prev: ({startTimeMin: -1});
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							width: "%"
							text: {
								var t = detail.data(detail.currentIndex, "startTimeMin");
								//console.warn(f1.prev, f1.prev.startTimeMin, t)
								if(t !== f1.prev.startTimeMin) {
									f1.prev.startTimeMin = t;
									return t;
								}
								return "";
							}
							textStyle: myStyle.textStyleBold
						}
					}
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						Cell {
							width: 12
							text: detail.data(detail.currentIndex, "classes.name");
						}
						Cell {
							width: "%"
							text: detail.data(detail.currentIndex, "competitorName");
						}
						Cell {
							width: 20
							text: detail.data(detail.currentIndex, "competitors.registration");
						}
						Cell {
							width: 20
							halign: Frame.AlignRight
							text: detail.data(detail.currentIndex, "runs.siId");
						}
						Cell {
							width: 15
							text: OGTime.msecToString_mmss(detail.data(detail.currentIndex, "startTimeMs"));
						}
					}
					//expandChildrenFrames: true
				}
			}
		}
	}
}


