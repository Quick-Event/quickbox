import qf.qmlreports 1.0
//import shared.QuickEvent.reports 1.

Report {
	id: root
	objectName: "root"
	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: MyStyle { id: myStyle }
		colors: [
		]
		pens: [
			/*
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			}
			*/
		]
	}
	textStyle: myStyle.textStyleDefault
	width: 297
	height: 210
	hinset: 5
	vinset: 5
	//layout: Frame.LayoutVertical
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		HeaderFooter {
			//reportTitle: root.reportTitle
		}
		Band {
			id: band
			vinset: 10
			width: "%"
			height: "%"
			createFromData: true
			Frame {
				objectName: "frame00"
				width: "%"
				Para {
					textFn: function() {return band.data("title");}
					textStyle: TextStyle {basedOn: "big"}
				}
			}
		}
	}
}


