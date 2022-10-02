import QtQml 2.0
import qf.qmlreports 1.0
import shared.qml.reports 1.0
import "qrc:/qf/core/qml/js/timeext.js" as TimeExt
import "qrc:/quickevent/core/js/ogtime.js" as OGTime

Frame {
	//visible: false
	hinset: 1
	vinset: 1
	Frame {
		width: "%"
		layout: Frame.LayoutHorizontal
		textStyle: myStyle.textStyleBold
		bottomBorder: Pen { basedOn: "black2" }
		htmlExportAttributes: {"lpt_textStyle": "bold,underline2"}
		Para {
			width: "%"
			text: detailCompetitor.data(detailCompetitor.currentIndex, "classes.name") + " " + detailCompetitor.data(detailCompetitor.currentIndex, "competitorName")
		}
		Para {
			textHAlign: Frame.AlignRight
			htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "right"}
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
		htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "center"}
		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "Slosovatelný kupón";
		}
		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "www.SUUNTOCZ.cz";
		}
		Frame { height: 2 }
		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "15% sleva pro závodníky.";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "na libovolné GPS hodinky značky Suunto.";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "Platí i na Suunto Ambit 3 Vertical za 2990,-";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "Platí do 31.7.2022  i na zlevněné produkty.";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "www.orienteering.cz";
		}
		Frame {
			layout: Frame.LayoutHorizontal
			valign: Frame.AlignVCenter
			Frame {
				width: "%"
				bottomBorder: Pen { basedOn: "black1dot" }
				//htmlExportAttributes: {"lpt_text": "-----"}
			}
			Para {
				text: "zde odstřihnout";
			}
			Frame {
				width: "%"
				bottomBorder: Pen { basedOn: "black1dot" }
				//htmlExportAttributes: {"lpt_text": "-----"}
			}
		}
	}
}
