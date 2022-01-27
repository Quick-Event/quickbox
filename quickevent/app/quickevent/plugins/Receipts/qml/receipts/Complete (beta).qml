import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.qml 1.0
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
import "qrc:/quickevent/core/js/ogtime.js" as OGTime
import "private" as Private

Report {
	id: root
	property int courseLength: 0
	property bool printLotteryTicket: false
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
			objectName: "band_competitor"
			modelData: "competitor"
			width: "%"
			Detail {
				id: detailCompetitor
				width: "%"
				layout: Frame.LayoutVertical
				bottomBorder: Pen { basedOn: "black2" }
				Private.LotteryTicket {
					visible: printLotteryTicket
				}
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
							return TimeExt.dateToISOString(bandCompetitor.data("event.date")) + " " + bandCompetitor.data("event.place")
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
							text: detailCompetitor.data(detailCompetitor.currentIndex, "competitorName")
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
							text: detailCompetitor.data(detailCompetitor.currentIndex, "classes.name") //"SI: " + detailCompetitor.data(detailCompetitor.currentIndex, "runs.siId")
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
			objectName: "band_card"
			modelData: "card"
			width: "%"
			Frame {
				hinset: 1
				width: "%"
				bottomBorder: Pen { basedOn: "black1" }
				htmlExportAttributes: {"lpt_borderBottom": "="}
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						id: paraCheck
						width: 12
						text: "Check:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "9", "lpt_textAlign": "right"}
						id: paraCheckTime
						width: 14
						textHAlign: Frame.AlignLeft
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToString_hhmmss(start00msec + bandCard.data("checkTimeMs"));
						}
					}
					Para {
						width: "%"
					}
					Cell {
						width: paraCheck.width
						text: "SI:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
						width: paraCheckTime.width
						textHAlign: Frame.AlignLeft
						textFn: function() { return bandCard.data("cardNumber"); }
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
						textHAlign: Frame.AlignLeft
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToString_hhmmss(start00msec + bandCard.data("startTimeMs"));
						}
					}
					Para {
						width: "%"
					}
					Cell {
						width: paraCheck.width
						text: "Finish:"
					}
					Para {
						htmlExportAttributes: {"lpt_textWidth": "9", "lpt_textAlign": "right"}
						width: paraCheckTime.width
						textHAlign: Frame.AlignLeft
						textFn: function() {
							var start00msec = bandCard.data("stageStartTimeMs");
							return TimeExt.msecToString_hhmmss(start00msec + bandCard.data("finishTimeMs"));
						}
					}
				}
			}
			Detail {
				id: dc
				width: "%"
				layout: Frame.LayoutHorizontal
				expandChildFrames: true
				fill: {
					var tm = data(dc.currentIndex, "stpTimeMs");
					if(!tm)
						return brushError;
					return brushNone;
				}
				topBorder: (dc.currentIndex < (dc.rowCount - 1))? null: myStyle.penBlack1
				htmlExportAttributes: (dc.currentIndex < (dc.rowCount - 2))? ({}): {"lpt_textStyle": "underline2"};

				Para {
					omitEmptyText: true
					textFn: function() {
						var is_last_row = (dc.currentIndex == (dc.rowCount - 1));
						cellPos.visible = !is_last_row;
						cellCodeOpen.visible = !is_last_row;
						cellCode.visible = !is_last_row;
						cellCodeClose.visible = !is_last_row;
						cellRunStatus.visible = is_last_row;
						return "";
					}
				}
				Para {
					id: cellPos
					htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "right"}
					width: 4
					textHAlign: Frame.AlignRight
					text: {
						var pos = dc.data(dc.currentIndex, "position");
						if(pos && dc.currentIndex < (dc.rowCount - 1))
							return pos;
						return "";
					}
				}
				Para {
					id: cellCodeOpen
					width: 2
					text: " ("
				}
				Para {
					id: cellCode
					htmlExportAttributes: {"lpt_textWidth": "6", "lpt_textAlign": "center"}
					width: 6
					textHAlign: Frame.AlignHCenter
					text: { return dc.data(dc.currentIndex, "code"); }
					textStyle: (dc.currentIndex < (dc.rowCount - 1))? null: myStyle.textStyleBold;
				}
				Para {
					id: cellCodeClose
					width: 1
					text: ")"
				}

				Para {
					width:  10
					id: cellRunStatus
					textHAlign: Frame.AlignHCenter
					textFn: function() { return bandCard.data("isOk")? qsTr("OK"): qsTr("DISQ"); }
					textStyle: (dc.currentIndex < (dc.rowCount - 1))? null: myStyle.textStyleBold;
				}

				Para {
					width: "%"
				}
				Para {
					id: cellStp
					htmlExportAttributes: {"lpt_textWidth": "13", "lpt_textAlign": "right"}
					width: (dc.currentIndex < (dc.rowCount - 1))? 12 : 14;
					textHAlign: Frame.AlignRight
					text: {
						var msec = dc.data(dc.currentIndex, "stpTimeMs");
						if(msec > 0)
							return " " + OGTime.msecToString_mmss(msec);
						return qsTr("-----");
					}
					textStyle: (dc.currentIndex < (dc.rowCount - 1) && dc.data(dc.currentIndex, "standCummulative") !== 1)? null: myStyle.textStyleBold;
				}

				Para {
					id: cellStandOpen
					width: 2
					textFn: function() {
						var is_last_row = dc.currentIndex == (dc.rowCount - 1);
						var msec = dc.data(dc.currentIndex, "lapTimeMs")
						if (msec > 0 && (!is_last_row || bandCard.data("isOk")))
							return " (";
						return  "";
					}
					textStyle: (dc.currentIndex < (dc.rowCount - 1) && dc.data(dc.currentIndex, "standCummulative") !== 1)? null: myStyle.textStyleBold;
				}
				Para {
					id: cellStand
					width: (dc.currentIndex < (dc.rowCount - 1))? 4 : 5;
					htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "center"}
					textHAlign: Frame.AlignHCenter
					textFn: function() {
						var msec = dc.data(dc.currentIndex, "lapTimeMs");
						var is_last_row = dc.currentIndex == (dc.rowCount - 1);
						var lap_stand = dc.data(dc.currentIndex, "standCummulative");
						if (msec  > 0 && (!is_last_row || bandCard.data("isOk")))
							return lap_stand;
						return "";
					}
					textStyle: (dc.currentIndex < (dc.rowCount - 1) && dc.data(dc.currentIndex, "standCummulative") !== 1)? null: myStyle.textStyleBold;
				}
				Para {
					id: cellStandClose
					width: 1
					textFn: function() {
						var is_last_row = dc.currentIndex == (dc.rowCount - 1);
						var msec = dc.data(dc.currentIndex, "lapTimeMs")
						if (msec > 0 && (!is_last_row || bandCard.data("isOk")))
							return ")";
						return  "";
					}
					textStyle: (dc.currentIndex < (dc.rowCount - 1) && dc.data(dc.currentIndex, "standCummulative") !== 1)? null: myStyle.textStyleBold;
				}

				Para {
					width: "%"
				}
				Para {
					id: cellLap
					htmlExportAttributes: {"lpt_textWidth": "10", "lpt_textAlign": "right"}
					width: 10
					textHAlign: Frame.AlignRight
					textFn: function() {
						var msec = dc.data(dc.currentIndex, "lapTimeMs");
						if(msec > 0)
							return OGTime.msecToString_mmss(msec);
						return qsTr("-----");
					}
					textStyle: (dc.data(dc.currentIndex, "standLap") !== 1)? null: myStyle.textStyleBold;
				}
				Para {
					id: cellLapStandOpen
					width: 2
					text: (dc.data(dc.currentIndex, "lapTimeMs") > 0) ? " (" : "";
					textStyle: (dc.data(dc.currentIndex, "standLap") !== 1)? null: myStyle.textStyleBold;
				}
				Para {
					id: cellLapStand
					width: 4
					htmlExportAttributes: {"lpt_textWidth": "4", "lpt_textAlign": "center"}
					textHAlign: Frame.AlignHCenter
					textFn: function() {
						var msec = dc.data(dc.currentIndex, "lapTimeMs");
						var lap_stand = dc.data(dc.currentIndex, "standLap");
						if(msec > 0)
							return lap_stand;
						return "";
					}
					textStyle: (dc.data(dc.currentIndex, "standLap") !== 1)? null: myStyle.textStyleBold;
				}
				Para {
					id: cellLapStandClose
					width: 1
					text: (dc.data(dc.currentIndex, "lapTimeMs") > 0) ? ")" : "";
					textStyle: (dc.data(dc.currentIndex, "standLap") !== 1)? null: myStyle.textStyleBold;
				}

				Para {
					width: "%"
				}
				Para {
					width: 2
					text: {
						var msec = dc.data(dc.currentIndex, "lossMs");
						if(msec > 0)
							return " +";
						return "";
					}
				}
				Para {
					htmlExportAttributes: {"lpt_textWidth": "11", "lpt_textAlign": "right"}
					id: cellLoss
					width: 9
					textHAlign: Frame.AlignRight
					text: {
						var msec = dc.data(dc.currentIndex, "lossMs");
						if(msec > 0)
							return OGTime.msecToString_mmss(msec);
						return "";
					}
				}
			}
			Frame {
				width: "%"
				bottomBorder: Pen { basedOn: "black2" }
				htmlExportAttributes: {"lpt_borderTop": "="}
				// fake para, just to run code and set visibility of cardLentFrame
				Para {
					omitEmptyText: true
					textFn: function() {
						var card_lent = bandCard.data("isCardLent");
						cardLentFrame.visible = card_lent
						var is_ok = bandCard.data("isOk");
						performanceInfoFrame.visible = is_ok;
						return "";
					}
				}
			}
			Frame {
				id: cardLentFrame
				//visible: false
				//vinset: 1
				htmlExportAttributes: {"lpt_borderBottom": "="}
				fill: brushError
				bottomBorder: Pen { basedOn: "black1" }
				textStyle: myStyle.textStyleBold
				Para {
					htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "center"}
					width: "%"
					textHAlign: Frame.AlignHCenter
					text: qsTr("!!! RENTED CARD !!!");
				}
			}

			Para {
				//vinset: 1
				hinset: 1
				textFn: function() {
					var bad_check = bandCard.data("isBadCheck");
					if(bad_check) {
						return qsTr("BAD CHECK !!!");
					}
					return "";
				}
			}
			Frame {
				id: performanceInfoFrame
				width: "%"
				//vinset: 1
				hinset: 1
				layout: Frame.LayoutVertical
				Para {
					textFn: function() {
						var current_standings = bandCard.data("currentStandings");
						var competitors_count = bandCard.data("competitorsFinished");
						if(current_standings && competitors_count)
							return qsTr("current placement = ") + current_standings + ". / " + competitors_count;
						return "";
					}
				}
				Para {
					textFn: function() {
						var dm = bandCard.dataModel;
						var loss = bandCard.data("timeMs") - bandCard.data("bestTime");
						return qsTr("loss to leading runner = ") + OGTime.msecToString_mmss(loss);
					}
				}
				Para {
					textFn: function() {
						var dm = bandCard.dataModel;
						var overall_loss = 0;
						//console.info("AHOJ", dm.dump());
						for(var i = 0; i < dm.rowCount(); i++) {
							var loss = dm.dataByName(i, 'lossMs');
							//console.info(i, loss);
							overall_loss += loss;
						}
						return qsTr("loss to superman =  ") + OGTime.msecToString_mmss(overall_loss);
					}
				}
				Para {
					textFn: function() {
						var time = bandCard.data("timeMs");
						var length = root.courseLength;
						if(length > 0)
							return qsTr("average pace = ") + OGTime.msecToString_mmss(((time / length) >> 0) * 1000) + " min/km";
						return "";
					}
				}
			}
			Para {
				//vinset: 1
				hinset: 1
				textFn: function() {
					var extra_codes = bandCard.data("extraCodes");
					//console.warn("missing_codes:", JSON.stringify(missing_codes, null, 2));
					if(extra_codes.length > 0) {
						var xcs = [];
						for(var i=0; i<extra_codes.length; i++) {
							var xca = extra_codes[i];
							xcs.push(xca[0] + "-" + xca[1]);
						}
						return qsTr("extra punches = ") + xcs.join(", ");
					}
					return "";
				}
			}
		}
	}
}


