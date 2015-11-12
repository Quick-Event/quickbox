import QtQml 2.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	property int courseLength: 0
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
			},
			Brush {
				id: brushError
				name: "error"
				color: Color {def:"salmon"}
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
			id: bandCompetitor
			modelData: "competitor"
			width: "%"
			Detail {
				id: detailCompetitor
				width: "%"
				layout: Frame.LayoutVertical
				bottomBorder: Pen { basedOn: "black2" }
				//LotteryTicket {}
				Frame {
					htmlExportAttributes: {"lpt_textAlign": "left", "lpt_borderTop": "=", "lpt_borderBottom": "-"}
					width: "%"
					fill: Brush { color: Color { def: "powderblue" } }
					topBorder: Pen { basedOn: "black2" }
					bottomBorder: Pen { basedOn: "black1" }
					hinset: 1
					vinset: 1
					Para {
						textFn: function() {
							var s = "";
							var stage_cnt = bandCompetitor.data("stageCount")
							if(stage_cnt > 1)
								s = qsTr("E") + bandCompetitor.data("currentStageId") + " - ";
							s += bandCompetitor.data("event.name")
							return s;
						}
					}
					Para {
						textFn: function() {
							return bandCompetitor.data("event.date").toISOString().substring(0, 10) + " " + bandCompetitor.data("event.place")
						}
					}
				}
				Frame {
					htmlExportAttributes: {"lpt_textStyle": "bold"}
					hinset: 1
					vinset: 1
					textStyle: myStyle.textStyleBold
					fill: Brush { color: Color { def: "khaki" } }
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						Para {
							width: "%"
							text: detailCompetitor.data(detailCompetitor.currentIndex, "classes.name") + " " + detailCompetitor.data(detailCompetitor.currentIndex, "competitorName")
						}
						Para {
							htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
							textHAlign: Frame.AlignRight
							text: {
								var t = detailCompetitor.data(detailCompetitor.currentIndex, "competitors.registration");
								return (t)? t: "NO_REG";
							}
						}
					}
					Frame {
						htmlExportAttributes: {"lpt_textStyle": "underline2"}
						width: "%"
						layout: Frame.LayoutHorizontal
						Para {
							htmlExportAttributes: {"lpt_textWidth": "%"}
							width: "%"
							//textHAlign: Frame.AlignLeft
							text: "SI: " + detailCompetitor.data(detailCompetitor.currentIndex, "runs.siId")
						}
						Para {
							textFn: function() {
								var l = bandCompetitor.data("courses.length");
								root.courseLength = (l)? l : 0;
								return (root.courseLength / 1000) + "km";
							}
						}
						Para {
							textFn: function() {
								return " " + bandCompetitor.data("courses.climb") + "m";
							}
						}
					}
				}
			}
		}
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
					layout: Frame.LayoutHorizontal
					Para {
						id: paraCheck
						width: 10
						text: "Check:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "9", "lpt_textAlign": "right"}
						id: paraCheckTime
						width: 14
						textHAlign: Frame.AlignRight
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToTimeString(start00msec + bandCard.data("checkTimeMs"));
						}
					}
					Cell {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: "%"
						textHAlign: Frame.AlignRight
						text: "Finish:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "9", "lpt_textAlign": "right"}
						width: paraCheckTime.width
						textHAlign: Frame.AlignRight
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToTimeString(start00msec + bandCard.data("finishTimeMs"));
						}
					}
				}
				Frame {
					htmlExportAttributes: {"lpt_textStyle": "underline2"}
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						width: paraCheck.width
						text: "Start:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "9", "lpt_textAlign": "right"}
						width: paraCheckTime.width
						textHAlign: Frame.AlignRight
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToTimeString(start00msec + bandCard.data("startTimeMs"));
						}
					}
					Cell {
						text: "/"
					}
					Para {
						textFn: function() {
							var start = bandCard.data("startTimeMs");
							return OGTime.msecToString(start)
						}
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
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
				fill: {
					var pos = data(dc.currentIndex, "position");
					if(!pos)
						return brushError;
					return brushNone;
				}
				Cell {
					id: cellPos
					htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "right"}
					width: 8
					textHAlign: Frame.AlignRight
					text: {
						var pos = dc.data(dc.currentIndex, "position");
						if(pos)
							return pos + ".";
						return "";
					}
				}
				Para {
					id: cellCode
					htmlExportAttributes: {"lpt_textWidth": "5", "lpt_textAlign": "right"}
					width: 10
					//textHAlign: Frame.AlignRight
					text: dc.data(dc.currentIndex, "code");
				}
				Para {
					id: cellStp
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "stpTimeMs"));
				}
				Para {
					id: cellLap
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lapTimeMs"));
				}
				Para {
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					id: cellLoss
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lossMs"));
				}
			}
			Frame {
				width: "%"
				htmlExportAttributes: {"lpt_borderTop": "-", "lpt_borderBottom": "="}
				textStyle: myStyle.textStyleBold
				topBorder: Pen { basedOn: "black1" }
				bottomBorder: Pen { basedOn: "black2" }
				Frame {
					htmlExportAttributes: {"lpt_textStyle": "bold"}
					layout: Frame.LayoutHorizontal
					Cell {
						htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "right"}
						width: cellPos.width
						textHAlign: Frame.AlignRight
						text: "FI:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "5", "lpt_textAlign": "right"}
						width: cellCode.width
						//textHAlign: Frame.AlignRight
						textFn: function() { return bandCard.data("isOk")? "OK": "DISK"; }
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: cellStp.width
						textHAlign: Frame.AlignRight
						textFn: function() { return OGTime.msecToString(bandCard.data("finishStpTimeMs")); }
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: cellLap.width
						textHAlign: Frame.AlignRight
						textFn: function() { return OGTime.msecToString(bandCard.data("finishLapTimeMs")); }
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: cellLoss.width
						textHAlign: Frame.AlignRight
						textFn: function() { return OGTime.msecToString(bandCard.data("finishLossMs")); }
					}
				}
			}
			Para {
				vinset: 1
				hinset: 1
				textFn: function() {
					var missing_codes = bandCard.data("missingCodes");
					//console.warn("missing_codes:", JSON.stringify(missing_codes, null, 2));
					if(missing_codes) {
						var mcs = [];
						for(var i=0; i<missing_codes.length; i++) {
							var mca = missing_codes[i];
							mcs.push(mca[0] + "-" + mca[1]);
						}
						var ret = qsTr("missing: ") + mcs.join(", ");
						return ret;
					}
					return ""
				}
			}
			Frame {
				width: "%"
				vinset: 1
				hinset: 1
				layout: Frame.LayoutHorizontal
				Para {
					textFn: function() {
						var current_standings = bandCard.data("currentStandings");
						var competitors_count = bandCard.data("competitorsFinished");
						var ret = qsTr("standings: ") + current_standings + "/" + competitors_count;
						return ret;
					}
				}
				Para {
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
					width: "%"
					textHAlign: Frame.AlignRight
					textFn: function() {
						var time = bandCard.data("finishStpTimeMs");
						var length = root.courseLength;
						if(length > 0)
							return OGTime.msecToString(((time / length) >> 0) * 1000) + "min/km";
						return "";
					}
				}
			}
		}
	}
}


