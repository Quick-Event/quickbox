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
				Frame {
					//visible: false
					hinset: 1
					vinset: 1
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						textStyle: myStyle.textStyleBold
						bottomBorder: Pen { basedOn: "black2" }
						Para {
							width: "%"
							text: detailCompetitor.data(detailCompetitor.currentIndex, "classes.name") + " " + detailCompetitor.data(detailCompetitor.currentIndex, "competitorName")
						}
						Para {
							textHAlign: Frame.AlignRight
							text: {
								var t = detailCompetitor.data(detailCompetitor.currentIndex, "competitors.registration");
								return (t)? t: "NO_REG";
							}
						}
					}
					Frame {
						width: "%"
						vinset: 1
						halign: Frame.AlignHCenter
						Para {
							textStyle: myStyle.textStyleBold
							text: "Slosovatelný kupón";
						}
						Para {
							textStyle: myStyle.textStyleBold
							text: "Každé vyhlášení - 3x hodinky";
						}
						Frame { height: 2 }
						Frame {
							layout: Frame.LayoutHorizontal
							valign: Frame.AlignVCenter
							Frame {
								width: "%"
								bottomBorder: Pen { basedOn: "black1dot" }
							}
							Para {
								text: "zde odstřihnout";
							}
							Frame {
								width: "%"
								bottomBorder: Pen { basedOn: "black1dot" }
							}
						}
						Frame { height: 2 }
						Para {
							textStyle: myStyle.textStyleBold
							text: "Čip + buzola = ComCard.";
						}
						Para {
							halign: Frame.AlignHCenter
							text: "Rychlé ražení.";
						}
						Para {
							halign: Frame.AlignHCenter
							text: "Žádné zastrkávání, stačí přiložit.";
						}
						Para {
							halign: Frame.AlignHCenter
							text: "Zvládnete to jedním prstem.";
						}
						Para {
							halign: Frame.AlignHCenter
							text: "Se slevou ve stánku HSH";
							textStyle: myStyle.textStyleBold
						}
					}
				}
				Frame {
					width: "%"
					fill: Brush { color: Color { def: "powderblue" } }
					topBorder: Pen { basedOn: "black2" }
					bottomBorder: Pen { basedOn: "black1" }
					hinset: 1
					vinset: 1
					Para {
						text: (bandCompetitor.modelLoaded)? bandCompetitor.data("event.name"): "";
					}
					Para {
						text: (bandCompetitor.modelLoaded)? bandCompetitor.data("event.date").toISOString().substring(0, 10)
														  + " " + bandCompetitor.data("event.place")
														  : "";
					}
				}
				Frame {
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
							textHAlign: Frame.AlignRight
							text: {
								var t = detailCompetitor.data(detailCompetitor.currentIndex, "competitors.registration");
								return (t)? t: "NO_REG";
							}
						}
					}
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						Para {
							width: "%"
							//textHAlign: Frame.AlignLeft
							text: "SI: " + detailCompetitor.data(detailCompetitor.currentIndex, "runs.siId")
						}
						Para {
							text: {
								if(bandCompetitor.modelLoaded) {
									var l = bandCompetitor.data("courses.length");
									root.courseLength = (l)? l : 0;
									return (root.courseLength / 1000) + "km";
								}
								return "";
							}
						}
						Para {
							text: (bandCompetitor.modelLoaded)? " " + bandCompetitor.data("courses.climb") + "m": "";
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
				hinset: 1
				width: "%"
				Frame {
					layout: Frame.LayoutHorizontal
					Para {
						id: paraCheck
						width: 12
						text: "Check:"
					}
					Para {
						id: paraCheckTime
						width: 15
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("checkTimeMs")): "";
					}
					Cell {
						width: "%"
						textHAlign: Frame.AlignRight
						text: "Finish:"
					}
					Para {
						width: paraCheckTime.width
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("finishTimeMs")): "";
					}
				}
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						width: paraCheck.width
						text: "Start:"
					}
					Para {
						width: paraCheckTime.width
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("startTimeMs")): "";
					}
					Cell {
						text: "/"
					}
					Para {
						text: {
							if(bandCard.modelLoaded) {
								var start = bandCard.data("startTimeMs");
								var start00 = bandCard.data("stageStart");
								start00 = TimeExt.msecSinceMidnight(start00);
								return OGTime.msecToString(start - start00)
							}
							return "---";
						}
					}
					Para {
						width: "%"
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? "SI:" + bandCard.data("cardNumber"): "";
					}
				}
				bottomBorder: Pen { basedOn: "black1" }
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
					width: 10
					//textHAlign: Frame.AlignRight
					text: dc.data(dc.currentIndex, "code");
				}
				Para {
					id: cellStp
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "stpTimeMs"));
				}
				Para {
					id: cellLap
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lapTimeMs"));
				}
				Para {
					id: cellLoss
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lossMs"));
				}
			}
			Frame {
				width: "%"
				textStyle: myStyle.textStyleBold
				topBorder: Pen { basedOn: "black1" }
				Frame {
					layout: Frame.LayoutHorizontal
					Cell {
						width: cellPos.width
						textHAlign: Frame.AlignRight
						text: "FI:"
					}
					Para {
						width: cellCode.width
						//textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? bandCard.data("isOk")? "OK": "DISK": "---";
					}
					Para {
						width: cellStp.width
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? OGTime.msecToString(bandCard.data("finishStpTimeMs")): "---";
					}
					Para {
						width: cellLap.width
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? OGTime.msecToString(bandCard.data("finishLapTimeMs")): "---";
					}
					Para {
						width: cellLoss.width
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? OGTime.msecToString(bandCard.data("finishLossMs")): "---";
					}
				}
				bottomBorder: Pen { basedOn: "black2" }
			}
			Para {
				vinset: 1
				hinset: 1
				text: {
					if(bandCard.modelLoaded) {
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
					}
					return "";
				}
			}
			Frame {
				width: "%"
				vinset: 1
				hinset: 1
				layout: Frame.LayoutHorizontal
				Para {
					text: {
						if(bandCard.modelLoaded) {
							var current_standings = bandCard.data("currentStandings");
							var competitors_count = bandCard.data("competitorsFinished");
							var ret = qsTr("standings: ") + current_standings + "/" + competitors_count;
							return ret;
						}
						return "";
					}
				}
				Para {
					width: "%"
					textHAlign: Frame.AlignRight
					text: {
						if(bandCard.modelLoaded) {
							var time = bandCard.data("finishStpTimeMs");
							var length = root.courseLength;
							if(length > 0)
								return OGTime.msecToString(((time / length) >> 0) * 1000) + "min/km";
						}
						return "";
					}
				}
			}
		}
	}
}


