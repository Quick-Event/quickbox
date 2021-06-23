import qf.qmlreports 1.0
//import reports 1.

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
	width: 210
	height: 297
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
			vinset: 5
			//hinset: 5
			width: "%"
			height: "%"
			createFromData: true
			Frame {
				objectName: "frame00"
				width: "%"
				vinset: 5
				Para {
					textFn: function() {return band.data("title");}
					textStyle: TextStyle {basedOn: "big"}
				}
				Para {
					textFn: function() {return band.data("note");}
				}
			}
		}
	}
}


