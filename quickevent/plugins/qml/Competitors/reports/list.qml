import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "root"
	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			}
		]
	}
	textStyle: myStyle.textStyleDefault
	width: 210
	height: 297
	vinset: 10
	hinset: 5
	//layout: Frame.LayoutVertical
	Band {
		id: band
		width: "%"
		height: "%"
		Frame {
			objectName: "frame00"
			width: "%"
			Para {
				textFn: function() {return band.data("title");}
				textStyle: TextStyle {basedOn: "big"}
			}
		}
		Frame {
			width: "%"
			layout: Frame.LayoutHorizontal
			fill: Brush {color: Color { def: "lightgray" }}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "ix"
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "class"
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "SI"
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "name"
			}
		}
		Detail {
			id: detail
			width: "%"
			layout: Frame.LayoutHorizontal
			expandChildrenFrames: true
			function dataFn(field_name) {return function() {return rowData(field_name);}}
			Para {
				width: "%"
				text: detail.currentIndex
			}
			Para {
				width: "%"
				textFn: detail.dataFn("classes.name");
			}
			Para {
				width: "%"
				textFn: function() {return detail.rowData("siId");}
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "competitorName")
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "competitorName")
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "competitorName")
			}
		}
	}
}


