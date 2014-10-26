import qf.qmlreports 1.0

Report {
	id: root
    objectName: "root"
    debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
        basedOn: MyStyle { id: myStyle }
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
    Frame {
        objectName: "frame00"
		width: "%"
//		height: "%"
        Para {
            text: "Title"
            textStyle: TextStyle {basedOn: "big"}
        }
	}
}


