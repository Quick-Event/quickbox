import qf.qmlreports 1.0

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
		Para {
			text: "Title"
			textStyle: TextStyle {basedOn: "big"}
		}
	}
	Band {
		width: "%"
		height: "%"
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
				text: "name"
			}
		}
		Detail {
			id: detail
			width: "%"
			layout: Frame.LayoutHorizontal
			Para {
				width: "%"
				text: detail.currentIndex
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "classId")
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "name")
			}
		}
	}
}


