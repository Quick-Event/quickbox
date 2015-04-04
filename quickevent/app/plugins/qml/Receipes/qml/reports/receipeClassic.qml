import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"
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

	width: 210 / 3
	height: 297
	hinset: 3
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
							text: detailCompetitor.data(detailCompetitor.currentIndex, "competitors.registration")
						}
					}
					Frame {
						width: "%"
						vinset: 1
						halign: Frame.AlignHCenter
						Para {
							text: "HSH actions";
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
						text: (bandCompetitor.modelLoaded)? bandCompetitor.data("event.date").toISOString().substring(0, 10): "";
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
							text: detailCompetitor.data(detailCompetitor.currentIndex, "competitors.registration")
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
							text: (bandCompetitor.modelLoaded)? (bandCompetitor.data("length") / 1000) + "km": "";
						}
						Para {
							text: (bandCompetitor.modelLoaded)? " " + bandCompetitor.data("climb") + "m": "";
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
						width: 15
						text: "Check:"
					}
					Para {
						width: "%"
						//textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("checkTimeMs")): "";
					}
					Para {
						width: paraCheck.width
						text: "Finish:"
					}
					Para {
						width: "%"
						//textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("finishTimeMs")): "";
					}
				}
				Frame {
					layout: Frame.LayoutHorizontal
					Para {
						width: paraCheck.width
						text: "Start:"
					}
					Para {
						width: 15
						textHAlign: Frame.AlignRight
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
						width: 20
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? TimeExt.msecToTimeString(bandCard.data("startTimeMs")): "";
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
				Cell {
					id: cellCode
					width: 9
					text: dc.data(dc.currentIndex, "code");
				}
				Cell {
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "stpTimeMs"));
				}
				Cell {
					width: "%"
					textHAlign: Frame.AlignRight
					text: OGTime.msecToString(dc.data(dc.currentIndex, "lapTimeMs"));
				}
				Cell {
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
						text: "FI:"
					}
					Cell {
						width: cellCode.width
						text: (bandCard.modelLoaded)? bandCard.data("isOk")? "OK": "DISK": "---";
					}
					Cell {
						width: "%"
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? OGTime.msecToString(bandCard.data("finishStpTimeMs")): "---";
					}
					Cell {
						width: "%"
						textHAlign: Frame.AlignRight
						text: (bandCard.modelLoaded)? OGTime.msecToString(bandCard.data("finishLapTimeMs")): "---";
					}
					Cell {
						width: "%"
						textHAlign: Frame.AlignRight
						text: "NIY";
					}
				}
				bottomBorder: Pen { basedOn: "black2" }
			}
		}
	}
}


