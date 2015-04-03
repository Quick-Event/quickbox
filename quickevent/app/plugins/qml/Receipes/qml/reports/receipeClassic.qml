import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"
	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon {
			id: myStyle
		}
	}
	textStyle: myStyle.textStyleDefault

	width: 210 / 3
	height: 297
	hinset: 5
	vinset: 5

	Frame {
		width: "%"
		Band {
			id: bandCompetitor
			modelData: "competitor"
			width: "%"
			Detail {
				id: d1
				width: "%"
				layout: Frame.LayoutVertical
				//expandChildrenFrames: true
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					Para {
						width: "%"
						text: d1.data(d1.currentIndex, "classes.name") + " " + d1.data(d1.currentIndex, "competitorName")
					}
					Para {
						textHAlign: Frame.AlignRight
						text: d1.data(d1.currentIndex, "competitors.registration")
					}
				}
			}
		}
	}
	Band {
		id: bandCard
		modelData: "card"
		width: "%"
		Detail {
			id: dc
			width: "%"
			layout: Frame.LayoutHorizontal
			expandChildrenFrames: true
			Cell {
				width: 10
				textHAlign: Frame.AlignRight
				text: dc.data(dc.currentIndex, "position") + 7 + ".";
			}
			Cell {
				width: 15
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
		}
	}
	/*
	Band {
		modelData: "competitor"
		bohuzel createFromData nefunguje pro card
		width: "%"
		createFromData: true
	}
	*/
}


