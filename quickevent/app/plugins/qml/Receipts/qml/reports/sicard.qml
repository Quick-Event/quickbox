import QtQml 2.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon {
			id: myStyle
		}
		brushes: [
			Brush {
				id: brushNone
				name: "none"
				color: Color {def:"#00000000"}
			}
		]
	}
	textStyle: myStyle.textStyleDefault

	width: 210 / 3 - 5
	height: 297
	hinset: 0
	vinset: 5

	Frame {
		width: "%"
		hinset: 2
		vinset: 2
		border: Pen { basedOn: "blue05" }
		Band {
			id: bandCard
			modelData: "card"
			width: "%"
			Frame {
				//htmlExportAttributes: {"lpt_borderBottom": "-"}
				hinset: 1
				width: "%"
				bottomBorder: Pen { basedOn: "black1" }
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						id: paraStart
						width: 15
						text: "Start:"
					}
					Para {
						width: "%"
						textFn: function() {
							var msec = bandCard.data("startTime") * 1000;
							return TimeExt.msecToTimeString(msec)
						}
					}
					Cell {
						//htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: paraStart.width
						textHAlign: Frame.AlignRight
						text: "Finish:"
					}
					Para {
						width: "%"
						textFn: function() {
							var msec = bandCard.data("finishTime") * 1000 + bandCard.data("finishTimeMs");
							return TimeExt.msecToTimeString(msec)
						}
					}
				}
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						width: paraStart.width
						text: "Check:"
					}
					Para {
						textHAlign: Frame.AlignRight
						textFn: function() {
							var msec = bandCard.data("checkTime") * 1000;
							return TimeExt.msecToTimeString(msec)
						}
					}
					Para {
						width: "%"
						textHAlign: Frame.AlignRight
						textFn: function() { return "SI:" + bandCard.data("cardNumber"); }
					}
				}
			}
			Detail {
				id: dc
				width: "%"
				layout: Frame.LayoutHorizontal
				expandChildrenFrames: true
				topBorder: (dc.currentIndex < (dc.rowCount - 1))? null: myStyle.penBlack1
				textStyle: (dc.currentIndex < (dc.rowCount - 1))? null: myStyle.textStyleBold;
				Cell {
					id: cellPos
					htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "right"}
					width: 8
					textHAlign: Frame.AlignRight
					text: {
						var pos = dc.data(dc.currentIndex, "position");
						if(pos && dc.currentIndex < (dc.rowCount - 1))
							return pos + ".";
						return "";
					}
				}
				Para {
					id: cellCode
					htmlExportAttributes: {"lpt_textWidth": "5", "lpt_textAlign": "right"}
					width: 10
					//textHAlign: Frame.AlignRight
					text: (dc.currentIndex < (dc.rowCount - 1))? dc.data(dc.currentIndex, "code"): qsTr("FI");
				}
				Para {
					id: cellStp
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					width: "%"
					textHAlign: Frame.AlignRight
					text: TimeExt.msecToTimeString(dc.data(dc.currentIndex, "stpTimeMs"));
				}
				Para {
					id: cellLap
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lapTimeMs"));
				}
			}
			Frame {
				width: "%"
				bottomBorder: Pen { basedOn: "black2" }
			}
			Frame {
				width: "%"
				vinset: 1
				hinset: 1
				layout: Frame.LayoutHorizontal
				Para {
					text: "Time: "
				}
				Para {
					width: "%"
					textFn: function() {
						var msec = bandCard.data("timeMs");
						return OGTime.msecToString(msec);
					}
				}
			}
		}
	}
}


