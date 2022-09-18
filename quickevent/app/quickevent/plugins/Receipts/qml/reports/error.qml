import QtQml 2.0
import qf.qmlreports 1.0
import shared.qml.reports 1.0

Report {
	id: root
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
			id: bandCard
			modelData: "card"
			width: "%"
			Frame {
				fill: brushError
				Para {
					htmlExportAttributes: {"lpt_textStyle": "bold", "lpt_textAlign": "center", "lpt_textWidth": "%"}
					width: "%"
					textStyle: myStyle.textStyleBold
					textHAlign: Frame.AlignHCenter
					topBorder: Pen { basedOn: "black2" }
					text: qsTr("!!! Error !!!");
				}
				Para {
					htmlExportAttributes: {"lpt_textAlign": "center", "lpt_borderBottom": "=", "lpt_textWidth": "%"}
					width: "%"
					bottomBorder: Pen { basedOn: "black2" }
					textStyle: myStyle.textStyleBold
					textHAlign: Frame.AlignHCenter
					textFn: function() { return "SI: " + bandCard.data("cardNumber"); }
				}
			}
		}
	}
}


