import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

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
				id: detail
				width: "%"
				layout: Frame.LayoutVertical
				//expandChildrenFrames: true
				Frame {
					layout: Frame.LayoutHorizontal
					Para {
						text: detail.data(detail.currentIndex, "competitors.registration") + " " + detail.data(detail.currentIndex, "competitorName")
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
			id: detailCard
			width: "%"
			layout: Frame.LayoutHorizontal
			expandChildrenFrames: true
			function dataFn(field_name) {return function() {return rowData(field_name);}}
			Cell {
				//width: colCode.renderedWidth
				textFn: detailCard.dataFn("code");
			}
			Cell {
				//width: colCode.renderedWidth
				text: detailCard.data(detailCard.currentIndex, "col1")
			}
			Cell {
				//width: colCode.renderedWidth
				textFn: detailCard.dataFn("col1")
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


