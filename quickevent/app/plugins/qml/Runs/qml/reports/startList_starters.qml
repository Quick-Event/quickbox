import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Start list for starters")
	property bool isPrintStartNumbers: false
	property int lineSpacing: 0
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

	width: root.options.isShirinkPageWidthToColumnCount? 210/2*root.options.columnCount: 210
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
			columns: {
				//console.info(JSON.stringify(root.options));
				root.options.columns
			}
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
						objectName: "minuteHeaderFrame"
						// hide recent value of start time in object to avoid property binding loop
						// property int prevStartTimeMin: -1 // property binding loop
						property var prev: ({startTimeMin: -1});
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							objectName: "minuteHeaderCell"
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
						objectName: "minuteFrame"
						width: "%"
						height: (root.lineSpacing > 0)? root.lineSpacing: undefined;
						layout: Frame.LayoutHorizontal
						Cell {
							objectName: "minuteCellClassName"
							width: 12
							text: detail.data(detail.currentIndex, "classes.name");
						}
						Cell {
							objectName: "minuteCellCompetitorName"
							width: "%"
							text: detail.data(detail.currentIndex, "competitorName");
						}
						Cell {
							objectName: "minuteCellRegistration"
							width: 20
							text: detail.data(detail.currentIndex, "competitors.registration");
						}
						Cell {
							objectName: "minuteCellSI"
							width: 20
							halign: Frame.AlignRight
							text: detail.data(detail.currentIndex, "runs.siId");
						}
						Cell {
							objectName: "minuteCellStartTimeMs"
							width: 15
							text: OGTime.msecToString_mmss(detail.data(detail.currentIndex, "startTimeMs"));
						}
					}
					/* strihani pro Zbyndu
					Frame {
						height: 5
					}
					*/
					//expandChildrenFrames: true
				}
			}
		}
	}
}


