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
		bottomBorder: Pen { basedOn: "black2" }
		htmlExportAttributes: {"lpt_textStyle": "bold,underline2"}
		Para {
			width: "%"
			textHAlign: Frame.AlignHCenter
			htmlExportAttributes: {"lpt_textStyle": "bold", "lpt_textWidth": "%", "lpt_textAlign": "center"}
		textStyle: myStyle.textStyleBold
			text: "Slosovatelný kupón";
		}
	}
	//vinset: 20
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
	Para { text: "" }
	Para { text: "" }
	Frame {
		layout: Frame.LayoutHorizontal
		valign: Frame.AlignVCenter
		Frame {
			width: "%"
			bottomBorder: Pen { basedOn: "black1dot" }
			htmlExportAttributes: {"lpt_textStyle": "underline2"}
			//htmlExportAttributes: {"lpt_text": "-----"}
		}
		Para {
			text: "zde odstřihnout";
			htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "center"}
		}
		Frame {
			width: "%"
			bottomBorder: Pen { basedOn: "black1dot" }
			//htmlExportAttributes: {"lpt_text": "-----"}
		}
	}
	Para { text: "" }
	Para { text: "" }
	Frame {
		width: "%"
		vinset: 1
		halign: Frame.AlignHCenter
		htmlExportAttributes: {"lpt_textWidth": "%", "lpt_textAlign": "center"}

		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "www.VJSPORT.cz";
		}
		Frame { height: 2 }
		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "podkolenky zdarma";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "jediné opravdové boty na OB s extrémní";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "výdrží. Nyní ve stánku HSH podkolenky";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "zdarma ke všem modelům kromě výprodeje";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "www.orienteering.cz";
		}
		Para {
			htmlExportAttributes: {"lpt_textStyle": "bold"}
			textStyle: myStyle.textStyleBold
			text: "Výsledky online";
		}
		Para {
			halign: Frame.AlignHCenter
			text: "oresults.eu/events/350";
		}
	}
}
